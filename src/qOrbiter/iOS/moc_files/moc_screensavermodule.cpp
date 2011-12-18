/****************************************************************************
** Meta object code from reading C++ file 'screensavermodule.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/screensaver/screensavermodule.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'screensavermodule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ScreenSaverModule[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       5,   79, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      34,   18,   18,   18, 0x05,
      54,   18,   18,   18, 0x05,
      69,   18,   18,   18, 0x05,
      87,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     112,   18,   18,   18, 0x0a,
     124,   18,   18,   18, 0x0a,
     146,   18,  141,   18, 0x0a,
     160,  158,   18,   18, 0x0a,
     176,   18,  141,   18, 0x0a,
     187,  158,   18,   18, 0x0a,
     206,   18,  202,   18, 0x0a,
     216,   18,   18,   18, 0x0a,

 // properties: name, type, flags
     233,  141, 0x11495003,
     247,  141, 0x11495003,
     272,  264, 0x0a095001,
     281,  202, 0x02495003,
     295,  202, 0x02495003,

 // properties: notify_signal_id
       0,
       1,
       0,
       3,
       4,

       0        // eod
};

static const char qt_meta_stringdata_ScreenSaverModule[] = {
    "ScreenSaverModule\0\0imageChanged()\0"
    "transitionChanged()\0loadingError()\0"
    "intervalChanged()\0transitionSpeedChanged()\0"
    "nextImage()\0nextTransition()\0QUrl\0"
    "getTImage()\0q\0setTImage(QUrl)\0getImage()\0"
    "setImage(QUrl)\0int\0getNext()\0"
    "startAlternate()\0current_image\0"
    "transition_image\0QString\0testtext\0"
    "timerInterval\0transitionInterval\0"
};

void ScreenSaverModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ScreenSaverModule *_t = static_cast<ScreenSaverModule *>(_o);
        switch (_id) {
        case 0: _t->imageChanged(); break;
        case 1: _t->transitionChanged(); break;
        case 2: _t->loadingError(); break;
        case 3: _t->intervalChanged(); break;
        case 4: _t->transitionSpeedChanged(); break;
        case 5: _t->nextImage(); break;
        case 6: _t->nextTransition(); break;
        case 7: { QUrl _r = _t->getTImage();
            if (_a[0]) *reinterpret_cast< QUrl*>(_a[0]) = _r; }  break;
        case 8: _t->setTImage((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 9: { QUrl _r = _t->getImage();
            if (_a[0]) *reinterpret_cast< QUrl*>(_a[0]) = _r; }  break;
        case 10: _t->setImage((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 11: { int _r = _t->getNext();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 12: _t->startAlternate(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ScreenSaverModule::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ScreenSaverModule::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScreenSaverModule,
      qt_meta_data_ScreenSaverModule, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ScreenSaverModule::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ScreenSaverModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ScreenSaverModule::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScreenSaverModule))
        return static_cast<void*>(const_cast< ScreenSaverModule*>(this));
    return QObject::qt_metacast(_clname);
}

int ScreenSaverModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QUrl*>(_v) = getImage(); break;
        case 1: *reinterpret_cast< QUrl*>(_v) = getTImage(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getTest(); break;
        case 3: *reinterpret_cast< int*>(_v) = getInterval(); break;
        case 4: *reinterpret_cast< int*>(_v) = getTInterval(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setImage(*reinterpret_cast< QUrl*>(_v)); break;
        case 1: setTImage(*reinterpret_cast< QUrl*>(_v)); break;
        case 3: setInterval(*reinterpret_cast< int*>(_v)); break;
        case 4: setTInterval(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ScreenSaverModule::imageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ScreenSaverModule::transitionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ScreenSaverModule::loadingError()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ScreenSaverModule::intervalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void ScreenSaverModule::transitionSpeedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
