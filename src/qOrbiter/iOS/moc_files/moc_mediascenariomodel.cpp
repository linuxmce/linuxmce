/****************************************************************************
** Meta object code from reading C++ file 'mediascenariomodel.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/mediascenariomodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mediascenariomodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MediaScenarioModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      40,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MediaScenarioModel[] = {
    "MediaScenarioModel\0\0topLeft,bottomRight\0"
    "DataChanged(QModelIndex,QModelIndex)\0"
    "handleItemChange()\0"
};

void MediaScenarioModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MediaScenarioModel *_t = static_cast<MediaScenarioModel *>(_o);
        switch (_id) {
        case 0: _t->DataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 1: _t->handleItemChange(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MediaScenarioModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MediaScenarioModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_MediaScenarioModel,
      qt_meta_data_MediaScenarioModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MediaScenarioModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MediaScenarioModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MediaScenarioModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MediaScenarioModel))
        return static_cast<void*>(const_cast< MediaScenarioModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int MediaScenarioModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void MediaScenarioModel::DataChanged(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
