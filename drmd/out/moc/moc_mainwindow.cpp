/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Jul 23 11:05:53 2020
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      45,   12,   11,   11, 0x08,
      92,   11,   11,   11, 0x08,
     102,   11,   11,   11, 0x08,
     117,   11,   11,   11, 0x08,
     130,   11,   11,   11, 0x08,
     156,  140,   11,   11, 0x08,
     192,  185,   11,   11, 0x08,
     207,   11,   11,   11, 0x08,
     237,  229,   11,   11, 0x08,
     263,  256,   11,   11, 0x08,
     279,   11,   11,   11, 0x08,
     293,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0type,start,current,next,end,door\0"
    "running_trigger(BYTE,BYTE,BYTE,BYTE,BYTE,bool)\0"
    "sysproc()\0movieChgProc()\0setWelcome()\0"
    "setInfo()\0mode,time,index\0"
    "setColorTest(BYTE,BYTE,BYTE)\0update\0"
    "setUpdate(int)\0MainDisplayType(BYTE)\0"
    "buf,len\0setSkip(BYTE*,int)\0mirror\0"
    "setMirror(bool)\0TimeoutProc()\0imageInit()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->running_trigger((*reinterpret_cast< BYTE(*)>(_a[1])),(*reinterpret_cast< BYTE(*)>(_a[2])),(*reinterpret_cast< BYTE(*)>(_a[3])),(*reinterpret_cast< BYTE(*)>(_a[4])),(*reinterpret_cast< BYTE(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        case 1: _t->sysproc(); break;
        case 2: _t->movieChgProc(); break;
        case 3: _t->setWelcome(); break;
        case 4: _t->setInfo(); break;
        case 5: _t->setColorTest((*reinterpret_cast< BYTE(*)>(_a[1])),(*reinterpret_cast< BYTE(*)>(_a[2])),(*reinterpret_cast< BYTE(*)>(_a[3]))); break;
        case 6: _t->setUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->MainDisplayType((*reinterpret_cast< BYTE(*)>(_a[1]))); break;
        case 8: _t->setSkip((*reinterpret_cast< BYTE*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->setMirror((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->TimeoutProc(); break;
        case 11: _t->imageInit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
