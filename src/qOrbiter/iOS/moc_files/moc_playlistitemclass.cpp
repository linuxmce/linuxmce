/****************************************************************************
** Meta object code from reading C++ file 'playlistitemclass.h'
**
** Created: Sat Dec 17 18:33:26 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/contextobjects/playlistitemclass.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playlistitemclass.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PlaylistItemClass[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      34,   18,   18,   18, 0x05,

 // methods: signature, parameters, type, tag, flags
      53,   18,   48,   18, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_PlaylistItemClass[] = {
    "PlaylistItemClass\0\0imageChanged()\0"
    "dataChanged()\0bool\0getActive()\0"
};

void PlaylistItemClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PlaylistItemClass *_t = static_cast<PlaylistItemClass *>(_o);
        switch (_id) {
        case 0: _t->imageChanged(); break;
        case 1: _t->dataChanged(); break;
        case 2: { bool _r = _t->getActive();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PlaylistItemClass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PlaylistItemClass::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PlaylistItemClass,
      qt_meta_data_PlaylistItemClass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PlaylistItemClass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PlaylistItemClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PlaylistItemClass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PlaylistItemClass))
        return static_cast<void*>(const_cast< PlaylistItemClass*>(this));
    return QObject::qt_metacast(_clname);
}

int PlaylistItemClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PlaylistItemClass::imageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void PlaylistItemClass::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
