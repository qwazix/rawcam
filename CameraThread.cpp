/** \file */

#include <qmetatype.h>
#include "CameraThread.h"
#include <QDebug>

#include <FCam/N9.h>

#include <vector>
#include <iostream>
#include <QQueue>
#include <qmath.h>

#include "OverlayWidget.h"
#include "CameraParameters.h"
//#include "LEDBlinker.h"

//SDK doesn't have these defines
#ifndef V4L2_CID_FLASH_LED_MODE
#define V4L2_CID_FLASH_LED_MODE 10225921
#endif

#ifndef V4L2_FLASH_LED_MODE_FLASH
#define V4L2_FLASH_LED_MODE_FLASH 1
#endif

using namespace std;

namespace Plat = FCam::N9;

void CameraThread::run() {
    // Make an asynchronous file writer to save images in the background

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

    // Action (Flash)
    FCam::Flash::FireAction fire(&flash);

    // Make a helper autofocus object
    FCam::AutoFocus autoFocus(&lens);

    // The viewfinder shot
    FCam::Shot viewfinder;
    viewfinder.exposure = int(parameters->exposure.value * 1000000 + 0.5);
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
    
    QQueue<QString> pictureNames;
//    LEDBlinker blinker;
//    LEDBlinker::BlinkAction blink(&blinker);

    while (keepGoing) {
	while (active) {
        //set flash parameters
        if (parameters->flash.mode == CameraParameters::Flash::FULL){
            if (viewfinder.exposure * 1000 > flash.minDuration()) fire.duration = flash.maxDuration();
            else fire.duration = flash.minDuration();
        } else {
            fire.duration = flash.minDuration();
        }
        if (parameters->flash.backCurtain == false) fire.time = 0; else fire.time = photo.exposure - fire.duration;
        if (parameters->flash.mode == CameraParameters::Flash::FULL){
            fire.brightness = flash.maxBrightness();
        } else {
            fire.brightness = flash.minBrightness();
        }

        // stream the viewfinder
        sensor.stream(viewfinder);
        if (focus && autoFocus.idle() && parameters->focus.mode == parameters->focus.AUTO) {
			autoFocus.startSweep();
			focus = false;
        } else if (parameters->focus.mode == parameters->focus.MANUAL){
            lens.setFocus(parameters->focus.value, lens.maxFocusSpeed());
        } else if (focus && parameters->focus.mode == parameters->focus.SPOT) {
            int x = parameters->focus.spot.x();
            int y = parameters->focus.spot.y();
//            qDebug()<<"Setting target to " << x << y;
            autoFocus.setTarget(FCam::Rect(x-15, y-15, 30, 30));
            autoFocus.startSweep();
            focus = false;
        }
		
		// Take a picture once autofocus completes and we have space to store the frame
		if (takeSnapshot && autoFocus.idle() && writer.savesPending() < 8) {
		    // use the metering the viewfinder has been doing
		    photo.exposure  = viewfinder.exposure;
		    photo.gain      = viewfinder.gain;
		    photo.whiteBalance = viewfinder.whiteBalance;
            if (parameters->flash.mode != CameraParameters::Flash::OFF) photo.addAction(fire);

		    sensor.capture(photo);
		    takeSnapshot = false;
		}

		// Drain the queue
		FCam::Frame f;
		do {
            f = sensor.getFrame();
            // filewriter doesn't emit a signal when it finishes saving a file so we use a hackish way;
            //qDebug()<< "before dequeure" <<  pictureNames;
            if (!pictureNames.isEmpty() && pictureNames.length()*2 > writer.savesPending()) emit pictureSaved(QString(pictureNames.dequeue()));

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


            char fname[256];
            // Save it as a JPEG
            snprintf(fname, 255, "%s/MyDocs/DCIM/photo_%s.jpg", getenv("HOME"),
                 f.exposureStartTime().toString().c_str());
            writer.saveJPEG(f, fname, 90);
            // filewriter doesn't emit a signal when it finishes saving a file so we use a hackish way;
            pictureNames.enqueue(QString(fname));
            //qDebug()<< "after enqueue" <<  pictureNames;

			// Save it as a DNG
            snprintf(fname, 255, "%s/MyDocs/DCIM/photo_%s.dng", getenv("HOME"),
                 f.exposureStartTime().toString().c_str());
            writer.saveDNG(f, fname);

		    } else if (f.shot().id == viewfinder.id) {

			// update the autofocus and metering algorithms
            autoFocus.update(f);

            autoExpose(&viewfinder, f, sensor.maxGain(),  sensor.maxExposure(), 0.5);

            if (parameters->exposure.mode == parameters->exposure.AUTO && parameters->gain.mode == parameters->gain.AUTO) {
                //we auto expose anyway and then modify the values so nothing left to be done here
                //and i dont want to reverse all the structure
            }
            else if (parameters->exposure.mode == parameters->exposure.MANUAL && parameters->gain.mode == parameters->gain.MANUAL) {
                viewfinder.exposure = int(parameters->exposure.value * 1000000 + 0.5);
                viewfinder.gain = parameters->gain.value;
            } else {
                if (parameters->exposure.mode == CameraParameters::Exposure::MANUAL) {
                    int newExp = int(1000000*parameters->exposure.value+0.5);
                    viewfinder.gain *= viewfinder.exposure;
                    viewfinder.gain /= newExp;
                    viewfinder.exposure = newExp;
                    // clamp to possible gains
                    if (viewfinder.gain < 1.0) viewfinder.gain = 1.0;
                    if (viewfinder.gain > 32.0) viewfinder.gain = 32.0;
                }

                if (parameters->gain.mode == CameraParameters::Gain::MANUAL) {
                    int newGain = parameters->gain.value;
                    viewfinder.exposure *= viewfinder.gain / newGain;
                    viewfinder.gain = newGain;
                }
            }
			autoWhiteBalance(&viewfinder, f);

            QString humanReadableExposure;
            if (viewfinder.exposure >= 1000000) {
                humanReadableExposure = QString::number(viewfinder.exposure /1000000) + "s";
            } else {
                humanReadableExposure = "1/" + QString::number(1000000 / (viewfinder.exposure));
            }
            emit exposureInfo(humanReadableExposure);
            emit gainInfo("ISO "+QString::number(int(viewfinder.gain*100)));
//            viewfinder.addAction(blink); //blink doesn't seem to work, must look into it
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
