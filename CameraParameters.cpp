#include "CameraParameters.h"
#include "math.h"
#include "QDebug"

CameraParameters::CameraParameters() {
    // Set default modes and values
    exposure.mode = Exposure::AUTO;
    exposure.value = 1.0f/30;
    gain.mode = Gain::AUTO;
    gain.value = 1.0f;
    focus.mode = Focus::AUTO;
    focus.value = 0.0f; // infinity
    whiteBalance.mode = WhiteBalance::AUTO;
    whiteBalance.value = 5000;
    burst.mode = Burst::SINGLE;
}

CameraParameters::~CameraParameters() {}

QString CameraParameters::Exposure::toString(float val) {
    QString str;

    if (val < 0.001) {
        str.sprintf("1/%d000s", (int)(0.001/val+0.5));
    } else if (val < 0.01) {
        str.sprintf("1/%d00s", (int)(0.01/val+0.5));
    } else if (val < 0.1) {
        str.sprintf("1/%d0s", (int)(0.1/val+0.5));
    } else if (val < 0.2) {
        str.sprintf("1/%ds", (int)(1/val+0.5));
    } else if (val < 0.95) {
        str.sprintf("0.%ds", (int)(10*val+0.5));
    } else {
        str.sprintf("%ds", (int)(val+0.5));
    }
    return str;
}

QString CameraParameters::Gain::toString(float val) {
    QString str;
    str.sprintf("ISO %d0", (int)(val*10 + 0.5));
    return str;
}

QString CameraParameters::Focus::toString(float val) {
    QString str;
    if (val > 5.0) { // up to 20cm
        str.sprintf("%dcm", (int)(100/val + 0.5));
    } else if (val > 1.0) { // up to 1m
        str.sprintf("%d0cm", (int)(10/val + 0.5));
    } else if (val > 0.2) { // up to 5m
        str.sprintf("%dm", (int)(1/val + 0.5));
    } else {
        str.sprintf(">5m");
    }
    return str;
}

QString CameraParameters::WhiteBalance::toString(float val) {
    QString str;
    str.sprintf("%d00K", (int)(val/100 + 0.5));
    return str;
}

void CameraParameters::setExposureMode(int mode){
    this->exposure.mode = mode;
}
void CameraParameters::setExposureModeAuto(){
    this->exposure.mode = Exposure::AUTO;
}
void CameraParameters::setExposureModeMan(){
    this->exposure.mode = Exposure::MANUAL;
}

void CameraParameters::setExposureValue(float val){
    this->exposure.value = val;
}

void CameraParameters::setExposureValue(int val){
    float left = 1/8000.0f;
    float middle = 1/30.0f;
    float right = 1.0f;
    float beta = log2f((right-left)/(middle-left));
    float alpha = (middle + right - 2*left)/(powf(500, beta)+powf(1000, beta));
    float gamma = left;
    this->exposure.value = powf((float)val, beta)*alpha + gamma;
}

<<<<<<< HEAD
void CameraParameters::setGainMode(int mode){
    this->gain.mode = mode;
}
void CameraParameters::setGainModeAuto(){
    this->gain.mode = Gain::AUTO;
}
void CameraParameters::setGainModeMan(){
    this->gain.mode = Gain::MANUAL;
}

void CameraParameters::setGainValue(float val){
    this->gain.value = val;
}

void CameraParameters::setGainValue(int val){
    this->gain.value = powf(2, val/200.0);
}


=======
>>>>>>> cca6f18522c9cada943789faa617be1fc221e039
void CameraParameters::setFocusMode(int mode){
    this->focus.mode = mode;
}
void CameraParameters::setFocusModeAuto(){
    this->focus.mode = Focus::AUTO;
}
void CameraParameters::setFocusModeMan(){
    this->focus.mode = Focus::MANUAL;
}
void CameraParameters::setFocusModeSpot(){
    this->focus.mode = Focus::SPOT;
}

void CameraParameters::setFocusValue(float val){
    this->focus.value = val;
}

void CameraParameters::setFocusValue(int val){
    this->focus.value = (1000-val)/50.0;
}

void CameraParameters::setFocusSpot(int x, int y){
    this->focus.spot.setX(x);
    this->focus.spot.setY(y);
    qDebug()<<"setFocusSpot: Setting target to " << x << y;
}


