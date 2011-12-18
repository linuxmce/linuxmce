/****************************************************************************
** Meta object code from reading C++ file 'filedetailsmodel.h'
**
** Created: Sat Dec 17 18:33:26 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/filedetailsmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filedetailsmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FileDetailsModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       2,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      56,   30,   17,   17, 0x05,
     107,   98,   17,   17, 0x05,
     132,   17,   17,   17, 0x05,
     151,  149,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     172,   17,   17,   17, 0x08,

 // methods: signature, parameters, type, tag, flags
     198,  191,   17,   17, 0x02,
     231,   17,  226,   17, 0x02,

 // properties: name, type, flags
     252,  226, 0x01495003,
     272,  264, 0x0a495103,

 // properties: notify_signal_id
       3,
       4,

       0        // eod
};

static const char qt_meta_stringdata_FileDetailsModel[] = {
    "FileDetailsModel\0\0ItemAdded()\0"
    "topLeft,bottomRight,state\0"
    "dataChanged(QModelIndex,QModelIndex,bool)\0"
    "pos,type\0SetTypeSort(int,QString)\0"
    "VisibleChanged()\0f\0FileChanged(QString)\0"
    "handleItemChange()\0format\0"
    "setSelectionStatus(QString)\0bool\0"
    "getSelectionStatus()\0showDetails\0"
    "QString\0file\0"
};

void FileDetailsModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FileDetailsModel *_t = static_cast<FileDetailsModel *>(_o);
        switch (_id) {
        case 0: _t->ItemAdded(); break;
        case 1: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3]))); break;
        case 2: _t->SetTypeSort((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->VisibleChanged(); break;
        case 4: _t->FileChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->handleItemChange(); break;
        case 6: _t->setSelectionStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: { bool _r = _t->getSelectionStatus();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FileDetailsModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FileDetailsModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_FileDetailsModel,
      qt_meta_data_FileDetailsModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FileDetailsModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FileDetailsModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FileDetailsModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileDetailsModel))
        return static_cast<void*>(const_cast< FileDetailsModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int FileDetailsModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isVisible(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getFile(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVisible(*reinterpret_cast< bool*>(_v)); break;
        case 1: setFile(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FileDetailsModel::ItemAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void FileDetailsModel::dataChanged(const QModelIndex & _t1, const QModelIndex & _t2, const bool & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FileDetailsModel::SetTypeSort(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FileDetailsModel::VisibleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void FileDetailsModel::FileChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
