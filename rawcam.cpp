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
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <QPropertyAnimation>

#include "CameraThread.h"
#include "ExampleOverlayWidget.h"
#include "myproximitysensor.h"
#include "CameraParameters.h"
#include "dot.h"

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
    QCoreApplication::setOrganizationName("oob");
    QCoreApplication::setApplicationName("rawcam");

    // Make the main window
    QWidget *window = new QWidget;
    QSettings settings;

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

    // gallery button
    QPushButton* gallery = new QPushButton("", window);
    gallery->move(780, 60);
    gallery->setObjectName("gallery");
    gallery->hide();

    // settings button
    QPushButton* settingsBtn = new QPushButton("", window);
    settingsBtn->move(780, 340);
    settingsBtn->setObjectName("settings");

    // shutter button
    QPushButton* shutter = new QPushButton(QIcon("/opt/rawcam/shutter.png"),"",window);
    shutter->setIconSize(QSize(90,90));
    shutter->move(750,200);
    shutter->setObjectName("shutter");
    QObject::connect(shutter, SIGNAL(pressed()), cameraThread, SLOT(focus_on_tap()));
    QObject::connect(shutter, SIGNAL(released()), cameraThread, SLOT(snapshot()));

//    t->setStyleSheet("min-height: 100px;"
//                          "border-style: outset;"
//                          "border-width: 2px;"
//                          "border-color: beige;" );




    MyProximitySensor *prx = new MyProximitySensor();
    QObject::connect(prx, SIGNAL(sensorClosed()), cameraThread, SLOT(focus_on()));
//    QObject::connect(prx, SIGNAL(sensorOpen()), cameraThread, SLOT(focus_off()));
    prx->start();

    //exposure info label
    QLabel* exposureInfo = new QLabel(window);
    exposureInfo->move(720,435);
    exposureInfo->setAlignment(Qt::AlignRight);
    exposureInfo->setFont(QFont("Nokia Pure Text", 20, 200, false));
    QObject::connect(cameraThread,SIGNAL(exposureInfo(QString)), exposureInfo, SLOT(setText(QString)));


    //gain info label
    QLabel* gainInfo = new QLabel(window);
    gainInfo->move(700,395);
    gainInfo->resize(120, 30);
    gainInfo->setAlignment(Qt::AlignRight);
    gainInfo->setFont(QFont("Nokia Pure Text", 20, 200, false));
    QObject::connect(cameraThread,SIGNAL(gainInfo(QString)), gainInfo, SLOT(setText(QString)));


    CameraParameters* params = cameraThread->parameters;

    QObject::connect(cameraThread, SIGNAL(pictureSaved(QString)), gallery, SLOT(show()));
    QObject::connect(cameraThread, SIGNAL(pictureSaved(QString)), params, SLOT(setLastPicture(QString)));
    QObject::connect(gallery,SIGNAL(clicked()), params, SLOT(openLastPicture()));

    // flash control
    QButtonGroup* flash = new QButtonGroup(window);
    flash->setExclusive(true);

    QPushButton* noflash = new QPushButton("", window);
    noflash->move(405, 15);
    noflash->setObjectName("noflash");
    noflash->setCheckable(true);
    noflash->setChecked(true);

    QPushButton* flashHalf = new QPushButton("", window);
    flashHalf->move(450, 15);
    flashHalf->setObjectName("flashHalf");
    flashHalf->setCheckable(true);
    flashHalf->setChecked(false);

    QPushButton* flashFull = new QPushButton("", window);
    flashFull->move(495, 15);
    flashFull->setObjectName("flashFull");
    flashFull->setCheckable(true);
    flashFull->setChecked(false);

    flash->addButton(noflash);
    flash->addButton(flashHalf);
    flash->addButton(flashFull);

    QPushButton* backCurtain = new QPushButton("", window);
    backCurtain->move(555, 15);
    backCurtain->setObjectName("backCurtain");
    backCurtain->setCheckable(true);
    backCurtain->setChecked(false);

    QObject::connect(noflash,SIGNAL(clicked()), params, SLOT(setFlashOff()));
    QObject::connect(flashHalf,SIGNAL(clicked()), params, SLOT(setFlashHalf()));
    QObject::connect(flashFull,SIGNAL(clicked()), params, SLOT(setFlashFull()));
    QObject::connect(backCurtain,SIGNAL(toggled(bool)), params, SLOT(setBackCurtain(bool)));

    // exposure control
    QButtonGroup* exp = new QButtonGroup(window);
    exp->setExclusive(true);


    QPushButton* p = new QPushButton("P", window);
    p->setFont(QFont("Nokia Pure Text", 30, 200, false));
    p->move(30, 10);
    p->setCheckable(true);
    p->setChecked(true);

    QPushButton* s = new QPushButton("S", window);
    s->setFont(QFont("Nokia Pure Text", 30, 200, false));
    s->move(90, 10);
    s->setCheckable(true);
    s->setChecked(false);

    QPushButton* iso = new QPushButton("I", window);
    iso->setFont(QFont("Nokia Pure Text", 30, 200, false));
    iso->move(150, 10);
    iso->setCheckable(true);
    iso->setChecked(false);

    QPushButton* m = new QPushButton("M", window);
    m->setFont(QFont("Nokia Pure Text", 30, 200, false));
    m->move(210,10);
    m->setCheckable(true);
    m->setChecked(false);

    exp->addButton(p);
    exp->addButton(m);
    exp->addButton(s);
    exp->addButton(iso);

    // exposure slider
    QSlider* expSlider = new QSlider(Qt::Horizontal, window);
    expSlider->resize(750,40);
    expSlider->move(80, 430);
    expSlider->hide();
    expSlider->setMinimum(0);
    expSlider->setMaximum(1000);
    expSlider->setValue(500);

    // gain slider
    QSlider* gainSlider = new QSlider(Qt::Horizontal, window);
    gainSlider->resize(750,40);
    gainSlider->move(80, 390);
    gainSlider->hide();
    gainSlider->setMinimum(0);
    gainSlider->setMaximum(1000);
    gainSlider->setValue(500);

    QObject::connect(p,SIGNAL(clicked()), expSlider, SLOT(hide()));
    QObject::connect(p,SIGNAL(clicked()), params, SLOT(setExposureModeAuto()));
    QObject::connect(p,SIGNAL(clicked()), gainSlider, SLOT(hide()));

    QObject::connect(m,SIGNAL(clicked()), params, SLOT(setExposureModeMan()));
    QObject::connect(m,SIGNAL(clicked()), expSlider, SLOT(show()));
    QObject::connect(m,SIGNAL(clicked()), params, SLOT(setGainModeMan()));
    QObject::connect(m,SIGNAL(clicked()), gainSlider, SLOT(show()));

    QObject::connect(s,SIGNAL(clicked()), expSlider, SLOT(show()));
    QObject::connect(s,SIGNAL(clicked()), gainSlider, SLOT(hide()));
    QObject::connect(s,SIGNAL(clicked()), params, SLOT(setExposureModeMan()));
    QObject::connect(s,SIGNAL(clicked()), params, SLOT(setGainModeAuto()));

    QObject::connect(iso,SIGNAL(clicked()), expSlider, SLOT(hide()));
    QObject::connect(iso,SIGNAL(clicked()), gainSlider, SLOT(show()));
    QObject::connect(iso,SIGNAL(clicked()), params, SLOT(setExposureModeAuto()));
    QObject::connect(iso,SIGNAL(clicked()), params, SLOT(setGainModeMan()));

    QObject::connect(expSlider,SIGNAL(valueChanged(int)), params, SLOT(setExposureValue(int)));
    QObject::connect(gainSlider,SIGNAL(valueChanged(int)), params, SLOT(setGainValue(int)));

    QObject::connect(cameraThread,SIGNAL(exposureChanged(int)), expSlider, SLOT(setValue(int)));
    QObject::connect(cameraThread,SIGNAL(gainChanged(int)), gainSlider, SLOT(setValue(int)));

    // focus control
    QButtonGroup* foc = new QButtonGroup(window);
    exp->setExclusive(true);

    QPushButton* af = new QPushButton("AF", window);
    af->setFont(QFont("Nokia Pure Text", 30, 200, false));
    af->move(670, 10);
    af->setCheckable(true);
    af->setChecked(true);

    QPushButton* sf = new QPushButton("", window);
    sf->move(725, 15);
    sf->setObjectName("sf");
    sf->setIconSize(QSize(32,32));
    sf->setCheckable(true);
    sf->setChecked(false);
    //focus dot (spot metering)
    dot* spot = new dot(" ", window);
    spot->setObjectName("spot");
    spot->hide();

    QPushButton* mf = new QPushButton("MF", window);
    mf->setFont(QFont("Nokia Pure Text", 30, 200, false));
    mf->move(780,10);
    mf->setCheckable(true);
    mf->setChecked(false);

    foc->addButton(af);
    foc->addButton(mf);
    foc->addButton(sf);

    // focus slider
    QSlider* focusSlider = new QSlider(Qt::Vertical, window);
    focusSlider->resize(40,400);
    focusSlider->move(18, 60);
    focusSlider->hide();
    focusSlider->setMinimum(0);
    focusSlider->setMaximum(1000);
    focusSlider->setValue(500);

    QObject::connect(af,SIGNAL(clicked()), focusSlider, SLOT(hide()));
    QObject::connect(af,SIGNAL(clicked()), params, SLOT(setFocusModeAuto()));
    QObject::connect(af,SIGNAL(clicked()), spot, SLOT(hide()));

    QObject::connect(mf,SIGNAL(clicked()), focusSlider, SLOT(show()));
    QObject::connect(mf,SIGNAL(clicked()), params, SLOT(setFocusModeMan()));
    QObject::connect(mf,SIGNAL(clicked()), spot, SLOT(hide()));

    QObject::connect(sf,SIGNAL(clicked()), focusSlider, SLOT(hide()));
    QObject::connect(sf,SIGNAL(clicked()), params, SLOT(setFocusModeSpot()));
    QObject::connect(sf,SIGNAL(clicked()), spot, SLOT(show()));

    QObject::connect(focusSlider,SIGNAL(valueChanged(int)), params, SLOT(setFocusValue(int)));

    // help label
    QPushButton* help = new QPushButton("cover the proximity \nsensor to focus", window);
    help->setFont(QFont("Nokia Pure Text", 20, 200, false));
    help->resize(320,80);
    help->setObjectName("help");
    if (settings.value("showHelp",true)==false) help->hide();

    QDeclarativeView *qmlView = new QDeclarativeView(window);
    qmlView->rootContext()->setContextProperty("params", params);
    qmlView->setSource(QUrl::fromLocalFile("/opt/rawcam/settings.qml"));
    qmlView->hide();
//    qmlView->resize(700,350);
    qmlView->move(60, 0);
    QObject::connect(settingsBtn,SIGNAL(clicked()), qmlView, SLOT(show()));
    //QObject::connect(overlay, SIGNAL(focus(int, int)), qmlView, SLOT(hide()));

    QPropertyAnimation openani(qmlView, "geometry");
    openani.setDuration(200);
    openani.setStartValue(QRect(60, 0, 700, 0));
    openani.setEndValue(QRect(60, 0, 700, 350));
    QObject::connect(settingsBtn,SIGNAL(clicked()), &openani, SLOT(start()));

    QPropertyAnimation closeani(qmlView, "geometry");
    closeani.setDuration(200);
    closeani.setStartValue(QRect(60, 0, 700, 350));
    closeani.setEndValue(QRect(60, 0, 700, 0));
    QObject::connect(overlay, SIGNAL(focus(int, int)), &closeani, SLOT(start()));
    QObject::connect(&closeani, SIGNAL(finished()), qmlView, SLOT(hide()));

    QTimer::singleShot(8000, help, SLOT(hide()));
    QObject::connect(help, SIGNAL(clicked()), help, SLOT(hide()));
    //settings.setValue("helpTimeout", 68);


    // Get the overlay to update when the camera has a new viewfinder
    // frame
    QObject::connect(cameraThread, SIGNAL(newViewfinderFrame()),
                     overlay, SLOT(newViewfinderFrame()));

    // Connect overlay signals to cameraThread slots
    QObject::connect(overlay, SIGNAL(focus(int, int)), params, SLOT(setFocusSpot(int, int)));

    //connect overlay touch to spot circle label
    QObject::connect(overlay, SIGNAL(focus(int, int)), spot, SLOT(move(int,int)));


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
    app.exec();

    while (cameraThread->writer.savesPending() > 0) sleep(1);
}

