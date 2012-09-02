#ifndef CAMERA_PARAMETERS_H
#define CAMERA_PARAMETERS_H

#include <QObject>
#include <QMutex>
#include <QString>
#include <QPoint>

/* The requested state of the camera. All communication between the UI
 * and the camera control thread passes through a single instance of
 * this class, which is owned by the camera control thread.
 *
 * The user interface reads this to display appropriate labels, and
 * modifies it when a mode is changed, or if you're in a manual mode
 * and sliders or other adjustment widgets are moved.
 *
 * The camera thread reads this to guide its behavior, and updates the
 * values when in an automatic mode.
*/
class CameraParameters : public QObject {
    Q_OBJECT
public:
    CameraParameters();
    ~CameraParameters();

    struct Exposure {
        enum {AUTO = 0, MANUAL, HIGHLIGHTS, SHADOWS, AUTO_HDR};
        int mode;
        // exposure time in seconds
        float value;
        // If in HDR mode, how many shots to take
        int hdrShots;

        QString toString(float val);
    } exposure;

    struct Gain {
        enum {AUTO = 0, MANUAL};
        // Gain as a float from 1.0 to 32 (equivalent to ISO 100-3200)
        int mode;
        float value;

        QString toString(float val);
    } gain;

    struct Focus {
        enum {AUTO = 0, MANUAL, SPOT};
        int mode;
        // Focus in diopters
        float value;
        // pixel position in viewfinder frame for spot auto focus
        QPoint spot;

        QString toString(float val);
    } focus;

    struct WhiteBalance {
        enum {AUTO = 0, MANUAL,  POINT};
        int mode;
        // White balance in kelvin
        float value;

        QString toString(float val);
    } whiteBalance;

    struct Burst {
        // burst mode
        enum {SINGLE = 0, CONTINUOUS, SHARPEST};
        int mode;
    } burst;

    struct Flash {
        // burst mode
        enum {OFF = 0, HALF, FULL};
        int mode;
        bool backCurtain;
    } flash;

    QString lastPicture;

    // Emit the changed signal to notify other concerned objects that
    // the camera parameters have changed.
    void notify() {emit changed();}

    QMutex mutex;
    int static getExposureValue(float);


public slots:
    void setExposureMode(int);
    void setExposureModeAuto();
    void setExposureModeMan();
    void setExposureValue(float);
    void setExposureValue(int);

    void setGainMode(int);
    void setGainModeAuto();
    void setGainModeMan();
    void setGainValue(int);
    void setGainValue(float);

    void setFocusMode(int);
    void setFocusModeAuto();
    void setFocusModeMan();
    void setFocusModeSpot();
    void setFocusValue(float);
    void setFocusValue(int);
    void setFocusSpot(int, int);

    void setFlashOff();
    void setFlashHalf();
    void setFlashFull();
    void setBackCurtain(bool);

    void openLastPicture();
    void setLastPicture(QString);

signals:
    // A signal signifying that the camera parameters have changed.
    // The camera thread could connect to change it's behavior through
    // interrupts rather than polling as it does now. The viewfinder
    // connects to this signal to update the setting labels on the
    // parameter buttons appropriately.
    void changed();

};

#endif
