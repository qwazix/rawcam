#include "RawcamApp.h"
#include "QDebug"

RawcamApp::RawcamApp(int &argc, char *argv[])  :
    QApplication(argc, argv)
{
}

void RawcamApp::setCameraThread(CameraThread* ct){
    this->cameraThread = ct;
}

void RawcamApp::setOverlay(OverlayWidget* ol){
    this->overlay = ol;
}

void RawcamApp::quit(){
    qDebug()<<"preparing to exit";
    cameraThread->wait();
    delete overlay;
    delete cameraThread;
    QApplication::quit();
}
