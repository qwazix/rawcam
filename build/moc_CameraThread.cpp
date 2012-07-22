/****************************************************************************
** Meta object code from reading C++ file 'CameraThread.h'
**
** Created: Sun Jul 22 13:18:22 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CameraThread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CameraThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CameraThread[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   34,   34,   34, 0x05,
      35,   34,   34,   34, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   34,   34,   34, 0x0a,
      64,   34,   34,   34, 0x0a,
      72,   34,   34,   34, 0x0a,
      81,   34,   34,   34, 0x0a,
      92,   34,   34,   34, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CameraThread[] = {
    "CameraThread\0newViewfinderFrame()\0\0"
    "exposureInfo(QString)\0stop()\0pause()\0"
    "resume()\0focus_on()\0snapshot()\0"
};

const QMetaObject CameraThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CameraThread,
      qt_meta_data_CameraThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CameraThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CameraThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CameraThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CameraThread))
        return static_cast<void*>(const_cast< CameraThread*>(this));
    return QThread::qt_metacast(_clname);
}

int CameraThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newViewfinderFrame(); break;
        case 1: exposureInfo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: stop(); break;
        case 3: pause(); break;
        case 4: resume(); break;
        case 5: focus_on(); break;
        case 6: snapshot(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CameraThread::newViewfinderFrame()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CameraThread::exposureInfo(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
