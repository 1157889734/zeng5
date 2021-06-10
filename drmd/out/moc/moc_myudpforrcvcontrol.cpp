/****************************************************************************
** Meta object code from reading C++ file 'myudpforrcvcontrol.h'
**
** Created: Fri Apr 24 00:45:35 2020
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../myudpforrcvcontrol.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myudpforrcvcontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyUdpforRcvControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      42,   20,   19,   19, 0x05,
      99,   96,   19,   19, 0x05,
     123,  118,   19,   19, 0x05,
     152,  144,   19,   19, 0x05,
     176,  171,   19,   19, 0x05,
     208,  192,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     237,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyUdpforRcvControl[] = {
    "MyUdpforRcvControl\0\0type,n1,n2,n3,n4,door\0"
    "running_trigger_signal(BYTE,BYTE,BYTE,BYTE,BYTE,bool)\0"
    "n1\0Emerg_signal(BYTE)\0type\0"
    "setDisplayType(BYTE)\0buf,len\0"
    "setSkip(BYTE*,int)\0flag\0setMirror(bool)\0"
    "mode,time,index\0setColorTest(BYTE,BYTE,BYTE)\0"
    "mainSendProc()\0"
};

void MyUdpforRcvControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MyUdpforRcvControl *_t = static_cast<MyUdpforRcvControl *>(_o);
        switch (_id) {
        case 0: _t->running_trigger_signal((*reinterpret_cast< BYTE(*)>(_a[1])),(*reinterpret_cast< BYTE(*)>(_a[2])),(*reinterpret_cast< BYTE(*)>(_a[3])),(*reinterpret_cast< BYTE(*)>(_a[4])),(*reinterpret_cast< BYTE(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        case 1: _t->Emerg_signal((*reinterpret_cast< BYTE(*)>(_a[1]))); break;
        case 2: _t->setDisplayType((*reinterpret_cast< BYTE(*)>(_a[1]))); break;
        case 3: _t->setSkip((*reinterpret_cast< BYTE*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->setMirror((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->setColorTest((*reinterpret_cast< BYTE(*)>(_a[1])),(*reinterpret_cast< BYTE(*)>(_a[2])),(*reinterpret_cast< BYTE(*)>(_a[3]))); break;
        case 6: _t->mainSendProc(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MyUdpforRcvControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MyUdpforRcvControl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MyUdpforRcvControl,
      qt_meta_data_MyUdpforRcvControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyUdpforRcvControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyUdpforRcvControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyUdpforRcvControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyUdpforRcvControl))
        return static_cast<void*>(const_cast< MyUdpforRcvControl*>(this));
    return QObject::qt_metacast(_clname);
}

int MyUdpforRcvControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void MyUdpforRcvControl::running_trigger_signal(BYTE _t1, BYTE _t2, BYTE _t3, BYTE _t4, BYTE _t5, bool _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyUdpforRcvControl::Emerg_signal(BYTE _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MyUdpforRcvControl::setDisplayType(BYTE _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MyUdpforRcvControl::setSkip(BYTE * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MyUdpforRcvControl::setMirror(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MyUdpforRcvControl::setColorTest(BYTE _t1, BYTE _t2, BYTE _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
