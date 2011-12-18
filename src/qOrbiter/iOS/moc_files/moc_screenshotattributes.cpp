/****************************************************************************
** Meta object code from reading C++ file 'screenshotattributes.h'
**
** Created: Sat Dec 17 18:33:26 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/contextobjects/screenshotattributes.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'screenshotattributes.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_screenshotAttributes[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       3,   59, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,
      36,   21,   21,   21, 0x05,
      50,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      70,   21,   62,   21, 0x0a,
      96,   89,   21,   21, 0x0a,
     122,   21,   62,   21, 0x0a,
     149,  141,   21,   21, 0x0a,
     175,   21,   62,   21, 0x0a,
     198,  192,   21,   21, 0x0a,

 // properties: name, type, flags
     222,   62, 0x0a495001,
     236,   62, 0x0a495001,
     250,   62, 0x0a495001,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

static const char qt_meta_stringdata_screenshotAttributes[] = {
    "screenshotAttributes\0\0nameChanged()\0"
    "typeChanged()\0idChanged()\0QString\0"
    "getAttributeName()\0incAtr\0"
    "setAttributeName(QString)\0getAttributeType()\0"
    "incType\0setAttributeType(QString)\0"
    "getAttributeNo()\0attno\0setAttributeNo(QString)\0"
    "attributeName\0attributeType\0attributeNo\0"
};

void screenshotAttributes::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        screenshotAttributes *_t = static_cast<screenshotAttributes *>(_o);
        switch (_id) {
        case 0: _t->nameChanged(); break;
        case 1: _t->typeChanged(); break;
        case 2: _t->idChanged(); break;
        case 3: { QString _r = _t->getAttributeName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 4: _t->setAttributeName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: { QString _r = _t->getAttributeType();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 6: _t->setAttributeType((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: { QString _r = _t->getAttributeNo();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 8: _t->setAttributeNo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData screenshotAttributes::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject screenshotAttributes::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_screenshotAttributes,
      qt_meta_data_screenshotAttributes, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &screenshotAttributes::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *screenshotAttributes::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *screenshotAttributes::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_screenshotAttributes))
        return static_cast<void*>(const_cast< screenshotAttributes*>(this));
    return QObject::qt_metacast(_clname);
}

int screenshotAttributes::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getAttributeName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getAttributeType(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getAttributeNo(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void screenshotAttributes::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void screenshotAttributes::typeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void screenshotAttributes::idChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
