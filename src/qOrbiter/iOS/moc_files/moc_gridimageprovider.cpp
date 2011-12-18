/****************************************************************************
** Meta object code from reading C++ file 'gridimageprovider.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/imageProviders/gridimageprovider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gridimageprovider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GridIndexProvider[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      44,   19,   18,   18, 0x0a,
     102,   85,   18,   18, 0x0a,
     135,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GridIndexProvider[] = {
    "GridIndexProvider\0\0topLeft,bottomRight,cRow\0"
    "dataUpdated(QModelIndex,QModelIndex,int)\0"
    "parent,start,end\0dataDeleted(QModelIndex,int,int)\0"
    "dataReset()\0"
};

void GridIndexProvider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GridIndexProvider *_t = static_cast<GridIndexProvider *>(_o);
        switch (_id) {
        case 0: _t->dataUpdated((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 1: _t->dataDeleted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->dataReset(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GridIndexProvider::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GridIndexProvider::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GridIndexProvider,
      qt_meta_data_GridIndexProvider, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GridIndexProvider::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GridIndexProvider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GridIndexProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GridIndexProvider))
        return static_cast<void*>(const_cast< GridIndexProvider*>(this));
    if (!strcmp(_clname, "QDeclarativeImageProvider"))
        return static_cast< QDeclarativeImageProvider*>(const_cast< GridIndexProvider*>(this));
    return QObject::qt_metacast(_clname);
}

int GridIndexProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
