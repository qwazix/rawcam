#ifndef RAWCAMAPP_H
#define RAWCAMAPP_H

#include <QApplication>
#include "CameraThread.h"

class RawcamApp : public QApplication
{
    Q_OBJECT
public:

    RawcamApp(int &argc, char *argv[]);
    void setCameraThread(CameraThread* ct);
    CameraThread* cameraThread;

public slots:
    void quit();
};

#endif // APP_H
