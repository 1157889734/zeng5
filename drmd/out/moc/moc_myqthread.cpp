/****************************************************************************
** Meta object code from reading C++ file 'myqthread.h'
**
** Created: Fri Apr 24 00:45:34 2020
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../myqthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myqthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_myqthread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      25,   10,   10,   10, 0x05,
      39,   10,   10,   10, 0x05,
      60,   55,   10,   10, 0x05,
      79,   10,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_myqthread[] = {
    "myqthread\0\0timeOut20ms()\0timeOut40ms()\0"
    "timeOut1000ms()\0flag\0ipConflictMsg(int)\0"
    "imageInit()\0"
};

void myqthread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        myqthread *_t = static_cast<myqthread *>(_o);
        switch (_id) {
        case 0: _t->timeOut20ms(); break;
        case 1: _t->timeOut40ms(); break;
        case 2: _t->timeOut1000ms(); break;
        case 3: _t->ipConflictMsg((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->imageInit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData myqthread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject myqthread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_myqthread,
      qt_meta_data_myqthread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &myqthread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *myqthread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *myqthread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_myqthread))
        return static_cast<void*>(const_cast< myqthread*>(this));
    return QThread::qt_metacast(_clname);
}

int myqthread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void myqthread::timeOut20ms()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void myqthread::timeOut40ms()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void myqthread::timeOut1000ms()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void myqthread::ipConflictMsg(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void myqthread::imageInit()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
