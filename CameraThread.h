#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

/** \file */

#include <QThread>
#include <QSemaphore>
#include <stdio.h>
#include <FCam/N9.h>
#include "CameraParameters.h"
#include <QString>
#include <QSettings>

class OverlayWidget;

class CameraThread : public QThread {
    Q_OBJECT;

public:
    CameraParameters* parameters;
    FCam::AsyncFileWriter writer;
    CameraThread(OverlayWidget *o, QObject *parent = NULL) : QThread(parent) {
        overlay = o;
        keepGoing = true;
	focus = false;
	active = true;
	cameralock = new QSemaphore(1);
    parameters = new CameraParameters;

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

    void focus_on_tap() {
        QSettings settings;
        if(settings.value("focusOnTap",false) == true) focus = true;
    }
	    
    void snapshot() {
        takeSnapshot = true;
    }

signals:
    void newViewfinderFrame();
    void exposureInfo(QString);
    void exposureChanged(int);
    void gainInfo(QString);
    void gainChanged(int);
    void pictureSaved(QString);

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
