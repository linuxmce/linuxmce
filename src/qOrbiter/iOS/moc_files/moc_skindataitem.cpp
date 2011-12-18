/****************************************************************************
** Meta object code from reading C++ file 'skindataitem.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/skindataitem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'skindataitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SkinDataItem[] = {

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
      14,   13,   13,   13, 0x05,
      28,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SkinDataItem[] = {
    "SkinDataItem\0\0dataChanged()\0imageChanged()\0"
};

void SkinDataItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SkinDataItem *_t = static_cast<SkinDataItem *>(_o);
        switch (_id) {
        case 0: _t->dataChanged(); break;
        case 1: _t->imageChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SkinDataItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SkinDataItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SkinDataItem,
      qt_meta_data_SkinDataItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SkinDataItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SkinDataItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SkinDataItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SkinDataItem))
        return static_cast<void*>(const_cast< SkinDataItem*>(this));
    return QObject::qt_metacast(_clname);
}

int SkinDataItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SkinDataItem::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SkinDataItem::imageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
