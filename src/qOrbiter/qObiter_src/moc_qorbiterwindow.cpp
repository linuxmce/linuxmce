/****************************************************************************
** Meta object code from reading C++ file 'qorbiterwindow.h'
**
** Created: Mon Jun 20 10:59:03 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qmlapplicationviewer/qorbiterwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qorbiterwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qOrbiterWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   31,   15,   15, 0x0a,
      57,   15,   15,   15, 0x0a,

 // methods: signature, parameters, type, tag, flags
      79,   15,   69,   15, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_qOrbiterWindow[] = {
    "qOrbiterWindow\0\0orbiterReady()\0Qmsg\0"
    "sendMessage(QString)\0swapSkins()\0"
    "QDateTime\0getCurrentDateTime()\0"
};

const QMetaObject qOrbiterWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_qOrbiterWindow,
      qt_meta_data_qOrbiterWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &qOrbiterWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *qOrbiterWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *qOrbiterWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qOrbiterWindow))
        return static_cast<void*>(const_cast< qOrbiterWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int qOrbiterWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: orbiterReady(); break;
        case 1: sendMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: swapSkins(); break;
        case 3: { QDateTime _r = getCurrentDateTime();
            if (_a[0]) *reinterpret_cast< QDateTime*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void qOrbiterWindow::orbiterReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
