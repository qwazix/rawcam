#ifndef RAWCAMAPP_H
#define RAWCAMAPP_H

#include <QApplication>
#include "CameraThread.h"
#include "OverlayWidget.h"

class RawcamApp : public QApplication
{
    Q_OBJECT
public:

    RawcamApp(int &argc, char *argv[]);
    void setOverlay(OverlayWidget* ol);
    void setCameraThread(CameraThread* ct);
    CameraThread* cameraThread;
    OverlayWidget* overlay;

public slots:
    void quit();
};

#endif // APP_H
