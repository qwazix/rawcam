#include "myproximitysensor.h"
#include "QDebug"

MyProximitySensor::MyProximitySensor(QObject *parent) :
    QProximitySensor(parent)
{

    QObject::connect(this,SIGNAL(readingChanged()),this,SLOT(changeNotifier()));
}

void MyProximitySensor::changeNotifier() {
    if (this->reading()->close()) emit sensorClosed(); else emit sensorOpen();
}
