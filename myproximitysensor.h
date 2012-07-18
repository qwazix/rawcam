#ifndef MYPROXIMITYSENSOR_H
#define MYPROXIMITYSENSOR_H

#include <QProximitySensor>

QTM_USE_NAMESPACE

class MyProximitySensor : public QProximitySensor
{
    Q_OBJECT
public:
    explicit MyProximitySensor(QObject *parent = 0);
    
signals:
    void sensorClosed();
    void sensorOpen();
    
public slots:
    void changeNotifier();
};

#endif // MYPROXIMITYSENSOR_H
