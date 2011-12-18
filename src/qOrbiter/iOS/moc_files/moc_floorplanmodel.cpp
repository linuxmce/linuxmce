/****************************************************************************
** Meta object code from reading C++ file 'floorplanmodel.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/floorplanmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'floorplanmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FloorPlanModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       1,   29, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   15,   15,   15, 0x0a,

 // methods: signature, parameters, type, tag, flags
      63,   49,   15,   15, 0x02,

 // properties: name, type, flags
      95,   87, 0x0a495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

static const char qt_meta_stringdata_FloorPlanModel[] = {
    "FloorPlanModel\0\0pageChanged()\0"
    "handleItemChange()\0currentPageId\0"
    "setCurrentPage(QString)\0QString\0"
    "currentPage\0"
};

void FloorPlanModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FloorPlanModel *_t = static_cast<FloorPlanModel *>(_o);
        switch (_id) {
        case 0: _t->pageChanged(); break;
        case 1: _t->handleItemChange(); break;
        case 2: _t->setCurrentPage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FloorPlanModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FloorPlanModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_FloorPlanModel,
      qt_meta_data_FloorPlanModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FloorPlanModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FloorPlanModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FloorPlanModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FloorPlanModel))
        return static_cast<void*>(const_cast< FloorPlanModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int FloorPlanModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getCurrentPage(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCurrentPage(*reinterpret_cast< QString*>(_v)); break;
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
void FloorPlanModel::pageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
