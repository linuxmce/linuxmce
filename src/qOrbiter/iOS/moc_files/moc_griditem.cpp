/****************************************************************************
** Meta object code from reading C++ file 'griditem.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/griditem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'griditem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gridItem[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      25,    9,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_gridItem[] = {
    "gridItem\0\0imageChanged()\0dataChanged()\0"
};

void gridItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        gridItem *_t = static_cast<gridItem *>(_o);
        switch (_id) {
        case 0: _t->imageChanged(); break;
        case 1: _t->dataChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData gridItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gridItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_gridItem,
      qt_meta_data_gridItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gridItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gridItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gridItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gridItem))
        return static_cast<void*>(const_cast< gridItem*>(this));
    return QObject::qt_metacast(_clname);
}

int gridItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void gridItem::imageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void gridItem::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
