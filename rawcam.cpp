/** \file */

#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QFile>
#include <QTimer>

#include "CameraThread.h"
#include "ExampleOverlayWidget.h"
#include "myproximitysensor.h"

QTM_USE_NAMESPACE

/***********************************************************/
/* Full camera application                                 */
/*                                                         */
/* This example uses QT to create a full camera            */
/* application for the N9. It displays viewfinder frames   */
/* using an fbdev overlay.                                 */
/***********************************************************/

QString readFile(QString filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
     return NULL;
  }

  QByteArray total;
  QByteArray line;
  while (!file.atEnd()) {
     line = file.read(1024);
     total.append(line);
  }

  return QString(total);
}

//void QTest::qSleep(int ms)
//{
//    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
//    nanosleep(&ts, NULL);
//}



int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Make the main window
    QWidget *window = new QWidget;



    QHBoxLayout *layout = new QHBoxLayout(window);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
//    app.setStyleSheet(readFile("/opt/rawcam/style.css"));
    window->setStyleSheet(readFile("/opt/rawcam/style.css"));

    ExampleOverlayWidget *overlay = new ExampleOverlayWidget();

    // Make a thread that controls the camera
    CameraThread cameraThread(overlay);

    //overlay->setFixedSize(854, 480);
    layout->addWidget(overlay);
    QPushButton* w = new QPushButton();
    w->setFixedSize(1,480);
    w->setObjectName("w");
    //w->setStyleSheet("background-color:transparent");
    layout->addWidget(w);
    QPushButton* t = new QPushButton(QIcon("/opt/rawcam/shutter.png"),"",window);
    t->setIconSize(QSize(90,90));
    t->move(750,200);
    QObject::connect(t, SIGNAL(released()), &cameraThread, SLOT(snapshot()));

    QLabel* help = new QLabel("cover the proximity \nsensor to focus", window);
    help->setFont(QFont("Nokia Pure Text", 20, 200, false));
    help->resize(320,80);
    QTimer::singleShot(12000, help, SLOT(hide()));


//    t->setStyleSheet("min-height: 100px;"
//                          "border-style: outset;"
//                          "border-width: 2px;"
//                          "border-color: beige;" );




    MyProximitySensor *prx = new MyProximitySensor();
    QObject::connect(prx, SIGNAL(sensorClosed()), &cameraThread, SLOT(focus_on()));
    QObject::connect(prx, SIGNAL(sensorOpen()), &cameraThread, SLOT(focus_off()));
    prx->start();













    // Get the overlay to update when the camera has a new viewfinder
    // frame
    QObject::connect(&cameraThread, SIGNAL(newViewfinderFrame()),
                     overlay, SLOT(newViewfinderFrame()));

    // Connect overlay signals to cameraThread slots
//    QObject::connect(overlay, SIGNAL(focus()),
//                     &cameraThread, SLOT(focus_on()));


    // Once the camera thread stops, quit the app
    QObject::connect(&cameraThread, SIGNAL(finished()),
                     &app, SLOT(quit()));
    
    // Connect activate and deactivate events to camerathread
    QObject::connect(overlay, SIGNAL(deactivate()),
                     &cameraThread, SLOT(pause()));
    QObject::connect(overlay, SIGNAL(activate()),
                     &cameraThread, SLOT(resume()));

    QObject::connect(overlay, SIGNAL(quit()),
                     &cameraThread, SLOT(stop()));
    
    // Show the app full screen
    window->showFullScreen();

    // Launch the camera thread
    cameraThread.start();

    // Enter the QT main event loop
    return app.exec();
}

