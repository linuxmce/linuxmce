/****************************************************************************
** Meta object code from reading C++ file 'skindatamodel.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/skindatamodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'skindatamodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SkinDataModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   15,   14,   14, 0x0a,
      43,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SkinDataModel[] = {
    "SkinDataModel\0\0name\0setActiveSkin(QString)\0"
    "handleItemChange()\0"
};

void SkinDataModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SkinDataModel *_t = static_cast<SkinDataModel *>(_o);
        switch (_id) {
        case 0: _t->setActiveSkin((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->handleItemChange(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SkinDataModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SkinDataModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_SkinDataModel,
      qt_meta_data_SkinDataModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SkinDataModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SkinDataModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SkinDataModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SkinDataModel))
        return static_cast<void*>(const_cast< SkinDataModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int SkinDataModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
