/****************************************************************************
** Meta object code from reading C++ file 'sleepingalarm.h'
**
** Created: Sat Dec 17 18:33:27 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qOrbiter_src/datamodels/DataModelItems/sleepingalarm.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sleepingalarm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SleepingAlarm[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       7,  119, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      37,   14,   14,   14, 0x05,
      52,   14,   14,   14, 0x05,
      71,   14,   14,   14, 0x05,
      90,   14,   14,   14, 0x05,
     104,   14,   14,   14, 0x05,
     120,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     143,  134,   14,   14, 0x0a,
     163,   14,  159,   14, 0x0a,
     183,  176,   14,   14, 0x0a,
     203,   14,  198,   14, 0x0a,
     222,  214,   14,   14, 0x0a,
     249,   14,  241,   14, 0x0a,
     272,  261,   14,   14, 0x0a,
     294,   14,  241,   14, 0x0a,
     319,  309,   14,   14, 0x0a,
     340,   14,  241,   14, 0x0a,
     366,  354,   14,   14, 0x0a,
     389,   14,  241,   14, 0x0a,
     411,  405,   14,   14, 0x0a,
     428,   14,  241,   14, 0x0a,

 // properties: name, type, flags
     438,  159, 0x02495003,
     451,  241, 0x0a495103,
     461,  198, 0x01495103,
     467,  241, 0x0a495103,
     476,  241, 0x0a495003,
     487,  241, 0x0a495103,
     494,  241, 0x0a495103,

 // properties: notify_signal_id
       0,
       2,
       1,
       3,
       4,
       5,
       6,

       0        // eod
};

static const char qt_meta_stringdata_SleepingAlarm[] = {
    "SleepingAlarm\0\0eventHandlerChanged()\0"
    "stateChanged()\0alarmTimeChanged()\0"
    "countDownChanged()\0daysChanged()\0"
    "statusChanged()\0nameChanged()\0iHandler\0"
    "setHandler(int)\0int\0getHandler()\0"
    "iState\0setState(bool)\0bool\0getState()\0"
    "iStatus\0setStatus(QString)\0QString\0"
    "getStatus()\0iAlarmTime\0setAlarmTime(QString)\0"
    "getAlarmTime()\0iTimeLeft\0setTimeLeft(QString)\0"
    "getTimeLeft()\0iDaysActive\0"
    "setDaysActive(QString)\0getDaysActive()\0"
    "iName\0setName(QString)\0getName()\0"
    "eventHandler\0alarmTime\0state\0timeLeft\0"
    "activeDays\0status\0name\0"
};

void SleepingAlarm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SleepingAlarm *_t = static_cast<SleepingAlarm *>(_o);
        switch (_id) {
        case 0: _t->eventHandlerChanged(); break;
        case 1: _t->stateChanged(); break;
        case 2: _t->alarmTimeChanged(); break;
        case 3: _t->countDownChanged(); break;
        case 4: _t->daysChanged(); break;
        case 5: _t->statusChanged(); break;
        case 6: _t->nameChanged(); break;
        case 7: _t->setHandler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: { int _r = _t->getHandler();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 9: _t->setState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: { bool _r = _t->getState();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: _t->setStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 12: { QString _r = _t->getStatus();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 13: _t->setAlarmTime((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: { QString _r = _t->getAlarmTime();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 15: _t->setTimeLeft((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: { QString _r = _t->getTimeLeft();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 17: _t->setDaysActive((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: { QString _r = _t->getDaysActive();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 19: _t->setName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: { QString _r = _t->getName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SleepingAlarm::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SleepingAlarm::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SleepingAlarm,
      qt_meta_data_SleepingAlarm, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SleepingAlarm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SleepingAlarm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SleepingAlarm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SleepingAlarm))
        return static_cast<void*>(const_cast< SleepingAlarm*>(this));
    return QObject::qt_metacast(_clname);
}

int SleepingAlarm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = getHandler(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getAlarmTime(); break;
        case 2: *reinterpret_cast< bool*>(_v) = getState(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getTimeLeft(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getDaysActive(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getStatus(); break;
        case 6: *reinterpret_cast< QString*>(_v) = getName(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setHandler(*reinterpret_cast< int*>(_v)); break;
        case 1: setAlarmTime(*reinterpret_cast< QString*>(_v)); break;
        case 2: setState(*reinterpret_cast< bool*>(_v)); break;
        case 3: setTimeLeft(*reinterpret_cast< QString*>(_v)); break;
        case 4: setDaysActive(*reinterpret_cast< QString*>(_v)); break;
        case 5: setStatus(*reinterpret_cast< QString*>(_v)); break;
        case 6: setName(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void SleepingAlarm::eventHandlerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SleepingAlarm::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SleepingAlarm::alarmTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void SleepingAlarm::countDownChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void SleepingAlarm::daysChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void SleepingAlarm::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void SleepingAlarm::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
