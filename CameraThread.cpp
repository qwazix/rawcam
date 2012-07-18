/** \file */

#include <qmetatype.h>
#include "CameraThread.h"

#include <FCam/N9.h>

#include <vector>
#include <iostream>

#include "OverlayWidget.h"

using namespace std;

namespace Plat = FCam::N9;

void CameraThread::run() {
    // Make an asynchronous file writer to save images in the background
    FCam::AsyncFileWriter writer;
    Plat::Sensor sensor;
    Plat::Lens lens;
    Plat::Flash flash;

    // tell the sensor that the flash and the lens will be tagging
    // frames that come back from it
    sensor.attach(&flash);
    sensor.attach(&lens);

    // Explicitly power up the sensor
    if (sensor.initialize(0) == -1){
        printf("Error powering up the sensor.\n");
        return;
    }

    // Make a helper autofocus object
    FCam::AutoFocus autoFocus(&lens);

    // The viewfinder shot
    FCam::Shot viewfinder;
    viewfinder.exposure = 40000;
    viewfinder.gain = 1.0f;
    // run at 25 fps
    viewfinder.frameTime = 40000;
    // dump image data directly into the frame buffer
    viewfinder.image = overlay->framebuffer();
    // enable histograms and sharpness maps
    viewfinder.histogram.enabled = true;
    viewfinder.histogram.region = FCam::Rect(0, 0, 640, 480);
    viewfinder.sharpness.enabled = true;
    viewfinder.sharpness.size = FCam::Size(16, 12);

    // A full res photograph. We'll set the exposure, frameTime, and
    // gain later, after we meter. Default parameters apply (no
    // histograms or sharpness), image memory auto allocated for each
    // new photograph, so that we can have multiple unique photographs
    // saving at once.
    FCam::Shot photo;
    photo.image = FCam::Image(sensor.maxImageSize(), FCam::RAW, FCam::Image::AutoAllocate);
    
    while (keepGoing) {
	while (active) {
    		// stream the viewfinder
    		sensor.stream(viewfinder);
		if (focus && autoFocus.idle()) { 
			autoFocus.startSweep();
			focus = false;
		}
		
		// Take a picture once autofocus completes and we have space to store the frame
		if (takeSnapshot && autoFocus.idle() && writer.savesPending() < 8) {
		    // use the metering the viewfinder has been doing
		    photo.exposure  = viewfinder.exposure;
		    photo.gain      = viewfinder.gain;
		    photo.whiteBalance = viewfinder.whiteBalance;
		    sensor.capture(photo);
		    takeSnapshot = false;
		}

		// Drain the queue
		FCam::Frame f;
		do {
		    f = sensor.getFrame();

		    if (f.shot().id == photo.id) {
			// Our photo came back, asynchronously save it to disk
			// with a unique filename. We use the exposure start
			// time for now just so we don't have to keep a
			// globally unique numbering.
			if (!f.image().valid()) {
			    printf("ERROR: Photo dropped!\n");
			    continue;
			} else {
			    printf("Got a full-res frame\n");
			}

			// Save it as a DNG
			char fname[256];
            snprintf(fname, 255, "%s/MyDocs/DCIM/photo_%s.dng", getenv("HOME"),
				 f.exposureStartTime().toString().c_str());
			writer.saveDNG(f, fname);

			// Save it as a JPEG too
            snprintf(fname, 255, "%s/MyDocs/DCIM/photo_%s.jpg", getenv("HOME"),
				 f.exposureStartTime().toString().c_str());
			writer.saveJPEG(f, fname, 90);
		    } else if (f.shot().id == viewfinder.id) {

			// update the autofocus and metering algorithms
			autoFocus.update(f);
			autoExpose(&viewfinder, f, 88);
			autoWhiteBalance(&viewfinder, f);
			sensor.stream(viewfinder);

			emit newViewfinderFrame();
		    } else {
			printf("got some other frame\n");
		    }
		} while (sensor.framesPending());
	}
	sleep(1);
	sensor.stop();
	cameralock->acquire();
	cameralock->release();
    }
    // It is necessary to capture one "high resolution" image when exiting, otherwise camera-ui may not work.
    sensor.capture(photo);
    FCam::Frame fhr = sensor.getFrame();
    printf("got high res, exiting now\n");
}
