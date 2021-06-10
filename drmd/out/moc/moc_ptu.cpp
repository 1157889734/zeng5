/****************************************************************************
** Meta object code from reading C++ file 'ptu.h'
**
** Created: Fri Apr 24 00:45:38 2020
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ptu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ptu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ptu[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x05,
      17,    4,    4,    4, 0x05,
      31,    4,    4,    4, 0x05,
      45,    4,    4,    4, 0x05,
      60,    4,    4,    4, 0x05,
      75,    4,    4,    4, 0x05,
      90,    4,    4,    4, 0x05,

 // slots: signature, parameters, type, tag, flags
     105,    4,    4,    4, 0x0a,
     117,    4,    4,    4, 0x0a,
     132,    4,    4,    4, 0x0a,
     147,    4,    4,    4, 0x0a,
     161,    4,    4,    4, 0x0a,
     189,  176,    4,    4, 0x0a,
     229,  222,    4,    4, 0x0a,
     264,  256,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ptu[] = {
    "ptu\0\0timeout1s()\0timeout25ms()\0"
    "timeout50ms()\0timeout100ms()\0"
    "timeout150ms()\0timeout200ms()\0"
    "timeout500ms()\0connected()\0disconnected()\0"
    "tcpreadyRead()\0TimeoutSLOT()\0"
    "udpreadyRead()\0cmd,buff,len\0"
    "ptuTcpSendData(short,char*,uint)\0"
    "packet\0ptuTcpRecvProc(QByteArray)\0"
    "Ppacket\0ptuTcpPacketProc(ptuTcpRecvPacket)\0"
};

void ptu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ptu *_t = static_cast<ptu *>(_o);
        switch (_id) {
        case 0: _t->timeout1s(); break;
        case 1: _t->timeout25ms(); break;
        case 2: _t->timeout50ms(); break;
        case 3: _t->timeout100ms(); break;
        case 4: _t->timeout150ms(); break;
        case 5: _t->timeout200ms(); break;
        case 6: _t->timeout500ms(); break;
        case 7: _t->connected(); break;
        case 8: _t->disconnected(); break;
        case 9: _t->tcpreadyRead(); break;
        case 10: _t->TimeoutSLOT(); break;
        case 11: _t->udpreadyRead(); break;
        case 12: _t->ptuTcpSendData((*reinterpret_cast< short(*)>(_a[1])),(*reinterpret_cast< char*(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 13: _t->ptuTcpRecvProc((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 14: _t->ptuTcpPacketProc((*reinterpret_cast< const ptuTcpRecvPacket(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ptu::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ptu::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ptu,
      qt_meta_data_ptu, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ptu::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ptu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ptu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ptu))
        return static_cast<void*>(const_cast< ptu*>(this));
    return QThread::qt_metacast(_clname);
}

int ptu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void ptu::timeout1s()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ptu::timeout25ms()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ptu::timeout50ms()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ptu::timeout100ms()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void ptu::timeout150ms()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void ptu::timeout200ms()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void ptu::timeout500ms()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
