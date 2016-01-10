/****************************************************************************
** Meta object code from reading C++ file 'browserwindow.h'
**
** Created: Sat Jun 4 17:40:39 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../lmceBrowser/browserwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'browserwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_browserWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      30,   14,   14,   14, 0x05,
      43,   14,   14,   14, 0x05,
      53,   14,   14,   14, 0x05,
      65,   14,   14,   14, 0x05,
      77,   14,   14,   14, 0x05,
      91,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     109,  104,   14,   14, 0x0a,
     124,   14,   14,   14, 0x0a,
     136,   14,   14,   14, 0x0a,
     145,   14,   14,   14, 0x0a,
     154,   14,   14,   14, 0x0a,
     165,   14,   14,   14, 0x0a,
     175,   14,   14,   14, 0x09,
     191,  189,   14,   14, 0x09,
     208,   14,   14,   14, 0x09,
     233,   14,  228,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_browserWindow[] = {
    "browserWindow\0\0loadComplete()\0"
    "dceForward()\0dceBack()\0dceReload()\0"
    "dceFontUp()\0dceFontDown()\0dceUrl(QUrl)\0"
    "iUrl\0gotoPage(QUrl)\0goForward()\0"
    "goBack()\0fontUp()\0fontDown()\0bReload()\0"
    "adjustTitle()\0p\0setProgress(int)\0"
    "finishLoading(bool)\0bool\0keepLmAlive()\0"
};

const QMetaObject browserWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_browserWindow,
      qt_meta_data_browserWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &browserWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *browserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *browserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_browserWindow))
        return static_cast<void*>(const_cast< browserWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int browserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadComplete(); break;
        case 1: dceForward(); break;
        case 2: dceBack(); break;
        case 3: dceReload(); break;
        case 4: dceFontUp(); break;
        case 5: dceFontDown(); break;
        case 6: dceUrl((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 7: gotoPage((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 8: goForward(); break;
        case 9: goBack(); break;
        case 10: fontUp(); break;
        case 11: fontDown(); break;
        case 12: bReload(); break;
        case 13: adjustTitle(); break;
        case 14: setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: finishLoading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: { bool _r = keepLmAlive();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void browserWindow::loadComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void browserWindow::dceForward()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void browserWindow::dceBack()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void browserWindow::dceReload()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void browserWindow::dceFontUp()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void browserWindow::dceFontDown()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void browserWindow::dceUrl(QUrl _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
