#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

/** \file */

#include <QThread>
#include <QSemaphore>
#include <stdio.h>
#include <FCam/N9.h>

class OverlayWidget;

class CameraThread : public QThread {
    Q_OBJECT;

public:
    CameraThread(OverlayWidget *o, QObject *parent = NULL) : QThread(parent) {
        overlay = o;
        keepGoing = true;
	focus = false;
	active = true;
	cameralock = new QSemaphore(1);
    }

public slots:
    void stop() {
        keepGoing = false;
    }

    void pause() {
	if (active) {
		cameralock->acquire();
		active = false;
	}
		
    }

    void resume() {
	if (!active) {
		cameralock->release();
		active = true;
	}
    }

    void focus_on() {
        focus = true;
    }
	    
    void snapshot() {
        takeSnapshot = true;
    }

signals:
    void newViewfinderFrame();

protected:
    void run();

private:
    bool keepGoing;
    bool active;
    bool focus;
    bool takeSnapshot;
    OverlayWidget *overlay;
    QSemaphore *cameralock;
};

#endif
