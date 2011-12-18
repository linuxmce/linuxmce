/****************************************************************************
** Meta object code from reading C++ file 'mediatypemodel.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/mediatypemodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mediatypemodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MediaSubTypeModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      41,   19,   18,   18, 0x05,
      66,   18,   18,   18, 0x05,
     104,   78,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     146,   18,   18,   18, 0x08,

 // methods: signature, parameters, type, tag, flags
     172,  165,   18,   18, 0x02,
     205,   18,  200,   18, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_MediaSubTypeModel[] = {
    "MediaSubTypeModel\0\0pos,qs_sorting_string\0"
    "SetTypeSort(int,QString)\0ItemAdded()\0"
    "topLeft,bottomRight,state\0"
    "dataChanged(QModelIndex,QModelIndex,bool)\0"
    "handleItemChange()\0format\0"
    "setSelectionStatus(QString)\0bool\0"
    "getSelectionStatus()\0"
};

void MediaSubTypeModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MediaSubTypeModel *_t = static_cast<MediaSubTypeModel *>(_o);
        switch (_id) {
        case 0: _t->SetTypeSort((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->ItemAdded(); break;
        case 2: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3]))); break;
        case 3: _t->handleItemChange(); break;
        case 4: _t->setSelectionStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: { bool _r = _t->getSelectionStatus();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MediaSubTypeModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MediaSubTypeModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_MediaSubTypeModel,
      qt_meta_data_MediaSubTypeModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MediaSubTypeModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MediaSubTypeModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MediaSubTypeModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MediaSubTypeModel))
        return static_cast<void*>(const_cast< MediaSubTypeModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int MediaSubTypeModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void MediaSubTypeModel::SetTypeSort(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MediaSubTypeModel::ItemAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void MediaSubTypeModel::dataChanged(const QModelIndex & _t1, const QModelIndex & _t2, const bool & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
