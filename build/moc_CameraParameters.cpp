/****************************************************************************
** Meta object code from reading C++ file 'CameraParameters.h'
**
** Created: Sun Jul 22 19:29:48 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CameraParameters.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CameraParameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CameraParameters[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   27,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   27,   27,   27, 0x0a,
      49,   27,   27,   27, 0x0a,
      71,   27,   27,   27, 0x0a,
      92,   27,   27,   27, 0x0a,
     116,   27,   27,   27, 0x0a,
     138,   27,   27,   27, 0x0a,
     156,   27,   27,   27, 0x0a,
     175,   27,   27,   27, 0x0a,
     193,   27,   27,   27, 0x0a,
     212,   27,   27,   27, 0x0a,
     233,   27,   27,   27, 0x0a,
     252,  274,   27,   27, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CameraParameters[] = {
    "CameraParameters\0changed()\0\0"
    "setExposureMode(int)\0setExposureModeAuto()\0"
    "setExposureModeMan()\0setExposureValue(float)\0"
    "setExposureValue(int)\0setFocusMode(int)\0"
    "setFocusModeAuto()\0setFocusModeMan()\0"
    "setFocusModeSpot()\0setFocusValue(float)\0"
    "setFocusValue(int)\0setFocusSpot(int,int)\0"
    ",\0"
};

const QMetaObject CameraParameters::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CameraParameters,
      qt_meta_data_CameraParameters, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CameraParameters::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CameraParameters::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CameraParameters::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CameraParameters))
        return static_cast<void*>(const_cast< CameraParameters*>(this));
    return QObject::qt_metacast(_clname);
}

int CameraParameters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changed(); break;
        case 1: setExposureMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setExposureModeAuto(); break;
        case 3: setExposureModeMan(); break;
        case 4: setExposureValue((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: setExposureValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: setFocusMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setFocusModeAuto(); break;
        case 8: setFocusModeMan(); break;
        case 9: setFocusModeSpot(); break;
        case 10: setFocusValue((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 11: setFocusValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: setFocusSpot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void CameraParameters::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
