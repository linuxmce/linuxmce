/****************************************************************************
** Meta object code from reading C++ file 'film.h'
**
** Created: Tue Mar 8 17:19:02 2011
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "film.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'film.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_film[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x0a,
      34,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_film[] = {
    "film\0\0searchReply(QNetworkReply*)\0"
    "dataReply(QNetworkReply*)\0"
};

const QMetaObject film::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_film,
      qt_meta_data_film, 0 }
};

const QMetaObject *film::metaObject() const
{
    return &staticMetaObject;
}

void *film::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_film))
        return static_cast<void*>(const_cast< film*>(this));
    return QObject::qt_metacast(_clname);
}

int film::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: searchReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 1: dataReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
