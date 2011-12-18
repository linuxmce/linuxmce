/****************************************************************************
** Meta object code from reading C++ file 'epgchannellist.h'
**
** Created: Sat Dec 17 18:33:26 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/contextobjects/epgchannellist.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'epgchannellist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EPGChannelList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      53,   28,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     105,   98,   94,   15, 0x0a,
     127,   15,   94,   15, 0x2a,
     158,  147,  138,   15, 0x0a,
     186,  180,  138,   15, 0x2a,
     209,  204,   15,   15, 0x0a,
     240,  234,   15,   15, 0x0a,
     282,  273,   15,   15, 0x0a,
     327,  316,  311,   15, 0x0a,
     358,  354,  311,   15, 0x2a,
     390,  373,  311,   15, 0x0a,
     432,  422,  311,   15, 0x2a,
     466,  354,  452,   15, 0x0a,
     482,  479,  452,   15, 0x0a,
     508,  204,  496,   15, 0x0a,
     543,   15,  452,   15, 0x0a,
     556,   15,   15,   15, 0x0a,
     572,  567,  496,   15, 0x0a,
     597,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EPGChannelList[] = {
    "EPGChannelList\0\0ItemAdded()\0"
    "topLeft,bottomRight,sRow\0"
    "dataChanged(QModelIndex,QModelIndex,int)\0"
    "int\0parent\0rowCount(QModelIndex)\0"
    "rowCount()\0QVariant\0index,role\0"
    "data(QModelIndex,int)\0index\0"
    "data(QModelIndex)\0item\0appendRow(EPGItemClass*)\0"
    "items\0appendRows(QList<EPGItemClass*>)\0"
    "row,item\0insertRow(int,EPGItemClass*)\0"
    "bool\0row,parent\0removeRow(int,QModelIndex)\0"
    "row\0removeRow(int)\0row,count,parent\0"
    "removeRows(int,int,QModelIndex)\0"
    "row,count\0removeRows(int,int)\0"
    "EPGItemClass*\0takeRow(int)\0id\0"
    "find(QString)\0QModelIndex\0"
    "indexFromItem(const EPGItemClass*)\0"
    "currentRow()\0populate()\0name\0"
    "getChannelIndex(QString)\0handleItemChange()\0"
};

void EPGChannelList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EPGChannelList *_t = static_cast<EPGChannelList *>(_o);
        switch (_id) {
        case 0: _t->ItemAdded(); break;
        case 1: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 2: { int _r = _t->rowCount((*reinterpret_cast< const QModelIndex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: { int _r = _t->rowCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: { QVariant _r = _t->data((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 5: { QVariant _r = _t->data((*reinterpret_cast< const QModelIndex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 6: _t->appendRow((*reinterpret_cast< EPGItemClass*(*)>(_a[1]))); break;
        case 7: _t->appendRows((*reinterpret_cast< const QList<EPGItemClass*>(*)>(_a[1]))); break;
        case 8: _t->insertRow((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< EPGItemClass*(*)>(_a[2]))); break;
        case 9: { bool _r = _t->removeRow((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = _t->removeRow((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = _t->removeRows((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QModelIndex(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->removeRows((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { EPGItemClass* _r = _t->takeRow((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< EPGItemClass**>(_a[0]) = _r; }  break;
        case 14: { EPGItemClass* _r = _t->find((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< EPGItemClass**>(_a[0]) = _r; }  break;
        case 15: { QModelIndex _r = _t->indexFromItem((*reinterpret_cast< const EPGItemClass*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QModelIndex*>(_a[0]) = _r; }  break;
        case 16: { EPGItemClass* _r = _t->currentRow();
            if (_a[0]) *reinterpret_cast< EPGItemClass**>(_a[0]) = _r; }  break;
        case 17: _t->populate(); break;
        case 18: { QModelIndex _r = _t->getChannelIndex((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QModelIndex*>(_a[0]) = _r; }  break;
        case 19: _t->handleItemChange(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EPGChannelList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EPGChannelList::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_EPGChannelList,
      qt_meta_data_EPGChannelList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EPGChannelList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EPGChannelList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EPGChannelList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EPGChannelList))
        return static_cast<void*>(const_cast< EPGChannelList*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int EPGChannelList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void EPGChannelList::ItemAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void EPGChannelList::dataChanged(const QModelIndex & _t1, const QModelIndex & _t2, const int & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
