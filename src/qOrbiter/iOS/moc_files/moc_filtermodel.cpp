/****************************************************************************
** Meta object code from reading C++ file 'filtermodel.h'
**
** Created: Sat Dec 17 18:33:26 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/filtermodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filtermodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FilterModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      51,   25,   12,   12, 0x05,
     102,   93,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     127,   12,   12,   12, 0x08,

 // methods: signature, parameters, type, tag, flags
     157,  150,  146,   12, 0x02,
     179,   12,  146,   12, 0x22,
     202,  195,  190,   12, 0x02,
     230,  195,  190,   12, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_FilterModel[] = {
    "FilterModel\0\0ItemAdded()\0"
    "topLeft,bottomRight,state\0"
    "dataChanged(QModelIndex,QModelIndex,bool)\0"
    "pos,type\0SetTypeSort(int,QString)\0"
    "handleItemChange()\0int\0parent\0"
    "rowCount(QModelIndex)\0rowCount()\0bool\0"
    "format\0setSelectionStatus(QString)\0"
    "getSelectionStatus(QString)\0"
};

void FilterModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FilterModel *_t = static_cast<FilterModel *>(_o);
        switch (_id) {
        case 0: _t->ItemAdded(); break;
        case 1: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3]))); break;
        case 2: _t->SetTypeSort((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->handleItemChange(); break;
        case 4: { int _r = _t->rowCount((*reinterpret_cast< const QModelIndex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = _t->rowCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { bool _r = _t->setSelectionStatus((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: { bool _r = _t->getSelectionStatus((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FilterModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FilterModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_FilterModel,
      qt_meta_data_FilterModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FilterModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FilterModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FilterModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FilterModel))
        return static_cast<void*>(const_cast< FilterModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int FilterModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void FilterModel::ItemAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void FilterModel::dataChanged(const QModelIndex & _t1, const QModelIndex & _t2, const bool & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FilterModel::SetTypeSort(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
