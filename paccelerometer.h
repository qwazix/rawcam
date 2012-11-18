#ifndef PACCELEROMETER_H
#define PACCELEROMETER_H
#include <QAccelerometer>

// Neccessary for Qt Mobility API usage
QTM_USE_NAMESPACE

class pAccelerometer : public QObject, public QAccelerometerFilter
{
    Q_OBJECT

public:
    struct ReadingVal{
        qreal x;
        qreal y;
        qreal z;
    };

    pAccelerometer(QObject* parent = 0) : QObject(parent)
    {
        m_sensor = new QAccelerometer(this);
        m_sensor->addFilter(this);
        m_sensor->setDataRate(100);
        m_sensor->start();
    }
    ReadingVal reading(){
        return readingVal;
    }


private slots:

    // Override of QAcclerometerFilter::filter(QAccelerometerReading*)
    bool filter(QAccelerometerReading* reading)
    {
        qreal x = reading->x();
        qreal y = reading->y();
        qreal z = reading->z();

        readingVal.x = reading->x();
        readingVal.y = reading->y();
        readingVal.z = reading->z();

        // Process acceleration sensor readings ...

//        qDebug("Current device acceleration: x=%f y=%f z=%f", x, y, z);
        return true;
    }

private:
    ReadingVal readingVal;
    QAccelerometer* m_sensor;
};

#endif // PACCELEROMETER_H
