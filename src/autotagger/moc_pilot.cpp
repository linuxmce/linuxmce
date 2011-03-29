/****************************************************************************
** Meta object code from reading C++ file 'pilot.h'
**
** Created: Mon Mar 28 18:51:00 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "pilot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pilot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_tvshow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x0a,
      23,    7,    7,    7, 0x0a,
      39,    7,    7,    7, 0x0a,
      69,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_tvshow[] = {
    "tvshow\0\0dataFinished()\0setAttributes()\0"
    "replyFinished(QNetworkReply*)\0error()\0"
};

const QMetaObject tvshow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_tvshow,
      qt_meta_data_tvshow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &tvshow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *tvshow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *tvshow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_tvshow))
        return static_cast<void*>(const_cast< tvshow*>(this));
    return QObject::qt_metacast(_clname);
}

int tvshow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dataFinished(); break;
        case 1: setAttributes(); break;
        case 2: replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: error(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
