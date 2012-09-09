/** \file */

#include <qmetatype.h>
#include "CameraThread.h"
#include <QDebug>
#ifdef Q_WS_MAEMO_5
#include <FCam/N900.h>
#else
#include <FCam/N9.h>
#endif
#include <vector>
#include <iostream>
#include <QQueue>
#include <qmath.h>
#include <QSettings>

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
#ifdef Q_WS_MAEMO_5
namespace Plat = FCam::N900;
#else
namespace Plat = FCam::N9;
#endif

void CameraThread::run() {
    // Make an asynchronous file writer to save images in the background

    Plat::Sensor sensor;
    Plat::Lens lens;
    Plat::Flash flash;

    // tell the sensor that the flash and the lens will be tagging
    // frames that come back from it
    sensor.attach(&flash);
    sensor.attach(&lens);

#ifdef Q_WS_MAEMO_5
#else
    // Explicitly power up the sensor
    if (sensor.initialize(0) == -1){
        printf("Error powering up the sensor.\n");
        return;
    }
#endif

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
    viewfinder.whiteBalance = parameters->whiteBalance.value;

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
    #ifdef Q_WS_MAEMO_5
        FCam::Event e;
                while (FCam::getNextEvent(&e)) {
                    switch(e.type) {
                    case FCam::Event::Error:
                        printf("FCam Error: %s\n", e.description.c_str());
                        if (e.data == FCam::Event::DriverLockedError || e.data == FCam::Event::DriverMissingError) {
//                            emit panic(e);
                            sensor.stop();
//                            exitLock.lock();
                            printf("Terminating\n");
                            return;
                        }
                        break;
                    case FCam::Event::Warning:
                        printf("FCam Warning: %s\n", e.description.c_str());
                        break;
                    case FCam::Event::FocusPressed:
                        if (autoFocus.idle()) {
                            if (parameters->focus.mode == CameraParameters::Focus::AUTO) {
                                autoFocus.setTarget(FCam::Rect(0, 0, 640, 480));
                                autoFocus.startSweep();
                            } else if (parameters->focus.mode == CameraParameters::Focus::SPOT) {
                                int x = parameters->focus.spot.x();
                                int y = parameters->focus.spot.y();
                                printf("Setting target to %d %d\n", x, y);
                                autoFocus.setTarget(FCam::Rect(x-15, y-15, 30, 30));
                                autoFocus.startSweep();
                            }
                        }


//                        halfDepress = true;
                        break;
//                    case FCam::Event::FocusReleased:
//                        emit focusReleased();
//                        halfDepress = false;
//                        break;
                    case FCam::Event::ShutterPressed:
//                        emit shutterPressed();
                        takeSnapshot = true;
//                        fullDepress = true;
                        break;
//                    case FCam::Event::ShutterReleased:
//                        emit shutterReleased();
//                        fullDepress = false;
//                        break;
                    case FCam::Event::N900LensClosed:
                        emit lensCoverClosed();
                        break;
//                    case FCam::Event::N900LensOpened:
//                        emit lensCoverOpened();
//                        break;
                    };
                }
#endif

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
            if (!pictureNames.isEmpty() && pictureNames.length() > writer.savesPending()) {
                QString newFile = QString(pictureNames.dequeue());
                if (newFile.endsWith(".jpg")) emit pictureSaved(newFile);
            }

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

                QSettings settings;
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
                if (settings.value("saveDng",true)== true) {
                    writer.saveDNG(f, fname);
                    pictureNames.enqueue(QString(fname));
                }

		    } else if (f.shot().id == viewfinder.id) {

			// update the autofocus and metering algorithms
            autoFocus.update(f);

            autoExpose(&viewfinder, f, sensor.maxGain(),  sensor.maxExposure(), 0.5);//, parameters->exposure.compensation);

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
            if (parameters->whiteBalance.mode == parameters->whiteBalance.AUTO) autoWhiteBalance(&viewfinder, f);
            else viewfinder.whiteBalance = parameters->whiteBalance.value;

            QString humanReadableExposure;
            if (viewfinder.exposure >= 1000000) {
                humanReadableExposure = QString::number(viewfinder.exposure /1000000) + "s";
            } else {
                humanReadableExposure = "1/" + QString::number(1000000 / (viewfinder.exposure));
            }
            emit exposureInfo(humanReadableExposure);
            if (parameters->exposure.mode == parameters->exposure.AUTO)
                emit exposureChanged(CameraParameters::getExposureValue((viewfinder.exposure-0.5)/1000000));
            emit gainInfo("ISO "+QString::number(int(viewfinder.gain*100)));
//            viewfinder.addAction(blink); //blink doesn't seem to work, must look into it
            if (parameters->gain.mode == parameters->gain.AUTO)
                emit gainChanged((int)(200/(logf(2)/logf(viewfinder.gain))));
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
