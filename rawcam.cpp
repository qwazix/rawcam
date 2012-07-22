/** \file */

#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QFile>
#include <QTimer>
#include <QButtonGroup>
#include <QSlider>
#include <QDebug>
#include <QSettings>

#include "CameraThread.h"
#include "ExampleOverlayWidget.h"
#include "myproximitysensor.h"
#include "CameraParameters.h"

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
    QSettings settings("oob", "rawcam");

    QHBoxLayout *layout = new QHBoxLayout(window);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
//    app.setStyleSheet(readFile("/opt/rawcam/style.css"));
    window->setStyleSheet(readFile("/opt/rawcam/style.css"));

    ExampleOverlayWidget *overlay = new ExampleOverlayWidget();

    // Make a thread that controls the camera
    CameraThread* cameraThread = new CameraThread(overlay);

    //overlay->setFixedSize(854, 480);
    layout->addWidget(overlay);
    QPushButton* w = new QPushButton();
    w->setFixedSize(1,480);
    w->setObjectName("w");
    //w->setStyleSheet("background-color:transparent");
    layout->addWidget(w);

    QPushButton* shutter = new QPushButton(QIcon("/opt/rawcam/shutter.png"),"",window);
    shutter->setIconSize(QSize(90,90));
    shutter->move(750,200);
    shutter->setObjectName("shutter");
    QObject::connect(shutter, SIGNAL(released()), cameraThread, SLOT(snapshot()));




//    t->setStyleSheet("min-height: 100px;"
//                          "border-style: outset;"
//                          "border-width: 2px;"
//                          "border-color: beige;" );




    MyProximitySensor *prx = new MyProximitySensor();
    QObject::connect(prx, SIGNAL(sensorClosed()), cameraThread, SLOT(focus_on()));
    QObject::connect(prx, SIGNAL(sensorOpen()), cameraThread, SLOT(focus_off()));
    prx->start();

    // exposure control
    QButtonGroup* exp = new QButtonGroup(window);
    exp->setExclusive(true);


    QPushButton* p = new QPushButton("P", window);
    p->setFont(QFont("Nokia Pure Text", 30, 200, false));
    p->move(30, 10);
    p->setCheckable(true);
    p->setChecked(true);

    QPushButton* m = new QPushButton("M", window);
    m->setFont(QFont("Nokia Pure Text", 30, 200, false));
    m->move(90,10);
    m->setCheckable(true);
    m->setChecked(false);

    exp->addButton(p);
    exp->addButton(m);


    // exposure slider
    QSlider* expSlider = new QSlider(Qt::Horizontal, window);
    expSlider->resize(700,40);
    expSlider->move(20, 430);
    expSlider->hide();
    expSlider->setMinimum(0);
    expSlider->setMaximum(1000);
    expSlider->setValue(500);

    CameraParameters* params = cameraThread->parameters;
    qDebug()<<params;

    QObject::connect(p,SIGNAL(clicked()), expSlider, SLOT(hide()));
    QObject::connect(p,SIGNAL(clicked()), params, SLOT(setExposureModeAuto()));
    QObject::connect(m,SIGNAL(clicked()), expSlider, SLOT(show()));
    QObject::connect(m,SIGNAL(clicked()), params, SLOT(setExposureModeMan()));
    QObject::connect(expSlider,SIGNAL(valueChanged(int)), params, SLOT(setExposureValue(int)));


    // help label
    QPushButton* help = new QPushButton("cover the proximity \nsensor to focus", window);
    help->setFont(QFont("Nokia Pure Text", 20, 200, false));
    help->resize(320,80);
    help->setObjectName("help");

    QTimer::singleShot(8000, help, SLOT(hide()));
    QObject::connect(help, SIGNAL(clicked()), help, SLOT(hide()));
    //settings.setValue("helpTimeout", 68);

    //exposure info label
    QLabel* exposureInfo = new QLabel(window);
    exposureInfo->move(750,440);
    exposureInfo->setFont(QFont("Nokia Pure Text", 20, 200, false));
    QObject::connect(cameraThread,SIGNAL(exposureInfo(QString)), exposureInfo, SLOT(setText(QString)));


    // Get the overlay to update when the camera has a new viewfinder
    // frame
    QObject::connect(cameraThread, SIGNAL(newViewfinderFrame()),
                     overlay, SLOT(newViewfinderFrame()));

    // Connect overlay signals to cameraThread slots
//    QObject::connect(overlay, SIGNAL(focus()),
//                     &cameraThread, SLOT(focus_on()));


    // Once the camera thread stops, quit the app
    QObject::connect(cameraThread, SIGNAL(finished()),
                     &app, SLOT(quit()));
    
    // Connect activate and deactivate events to camerathread
    QObject::connect(overlay, SIGNAL(deactivate()),
                     cameraThread, SLOT(pause()));
    QObject::connect(overlay, SIGNAL(activate()),
                     cameraThread, SLOT(resume()));

    QObject::connect(overlay, SIGNAL(quit()),
                     cameraThread, SLOT(stop()));
    
    // Show the app full screen
    window->showFullScreen();

    // Launch the camera thread
    cameraThread->start();

    // Enter the QT main event loop
    return app.exec();
}

