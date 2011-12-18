/****************************************************************************
** Meta object code from reading C++ file 'orbiterwindow.h'
**
** Created: Sat Dec 17 18:33:26 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/orbiterwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'orbiterwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_orbiterWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   44, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      48,   32,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      85,   75,   14,   14, 0x0a,
     120,  115,   14,   14, 0x0a,
     148,   14,  140,   14, 0x0a,

 // methods: signature, parameters, type, tag, flags
     168,  161,   14,   14, 0x02,

 // properties: name, type, flags
     191,  140, 0x0a495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

static const char qt_meta_stringdata_orbiterWindow[] = {
    "orbiterWindow\0\0MessageChanged()\0"
    "device,routerIp\0setupLmce(QString,QString)\0"
    "device,ip\0qmlSetupLmce(QString,QString)\0"
    "imsg\0setMessage(QString)\0QString\0"
    "getMessage()\0forced\0forceResponse(QString)\0"
    "message\0"
};

void orbiterWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        orbiterWindow *_t = static_cast<orbiterWindow *>(_o);
        switch (_id) {
        case 0: _t->MessageChanged(); break;
        case 1: _t->setupLmce((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->qmlSetupLmce((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->setMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: { QString _r = _t->getMessage();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 5: _t->forceResponse((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData orbiterWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject orbiterWindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_orbiterWindow,
      qt_meta_data_orbiterWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &orbiterWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *orbiterWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *orbiterWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_orbiterWindow))
        return static_cast<void*>(const_cast< orbiterWindow*>(this));
    return QObject::qt_metacast(_clname);
}

int orbiterWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getMessage(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMessage(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void orbiterWindow::MessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void orbiterWindow::setupLmce(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
