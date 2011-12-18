/****************************************************************************
** Meta object code from reading C++ file 'listModel.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/listModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'listModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ListModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      48,   23,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      89,   10,   10,   10, 0x0a,
     104,   10,   10,   10, 0x08,

 // methods: signature, parameters, type, tag, flags
     143,  132,  123,   10, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_ListModel[] = {
    "ListModel\0\0ItemAdded()\0topLeft,bottomRight,sRow\0"
    "dataChanged(QModelIndex,QModelIndex,int)\0"
    "checkForMore()\0handleItemChange()\0"
    "QVariant\0index,name\0get(int,QString)\0"
};

void ListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ListModel *_t = static_cast<ListModel *>(_o);
        switch (_id) {
        case 0: _t->ItemAdded(); break;
        case 1: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 2: _t->checkForMore(); break;
        case 3: _t->handleItemChange(); break;
        case 4: { QVariant _r = _t->get((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_ListModel,
      qt_meta_data_ListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ListModel))
        return static_cast<void*>(const_cast< ListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int ListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ListModel::ItemAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ListModel::dataChanged(const QModelIndex & _t1, const QModelIndex & _t2, const int & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
