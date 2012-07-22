/****************************************************************************
** Meta object code from reading C++ file 'OverlayWidget.h'
**
** Created: Sun Jul 22 13:18:22 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../OverlayWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OverlayWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OverlayWidget[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   25,   25,   25, 0x05,
      26,   25,   25,   25, 0x05,
      39,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   25,   25,   25, 0x0a,
      67,   25,   25,   25, 0x0a,
      76,   25,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OverlayWidget[] = {
    "OverlayWidget\0activate()\0\0deactivate()\0"
    "quit()\0newViewfinderFrame()\0enable()\0"
    "disable()\0"
};

const QMetaObject OverlayWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_OverlayWidget,
      qt_meta_data_OverlayWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OverlayWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OverlayWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OverlayWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OverlayWidget))
        return static_cast<void*>(const_cast< OverlayWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int OverlayWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activate(); break;
        case 1: deactivate(); break;
        case 2: quit(); break;
        case 3: newViewfinderFrame(); break;
        case 4: enable(); break;
        case 5: disable(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void OverlayWidget::activate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void OverlayWidget::deactivate()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void OverlayWidget::quit()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
