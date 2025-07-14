/****************************************************************************
** Meta object code from reading C++ file 'pbsetup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../pbsetup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pbsetup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PBsetup_t {
    QByteArrayData data[9];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PBsetup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PBsetup_t qt_meta_stringdata_PBsetup = {
    {
QT_MOC_LITERAL(0, 0, 7), // "PBsetup"
QT_MOC_LITERAL(1, 8, 19), // "on_PBsetup_rejected"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 19), // "on_PBsetup_accepted"
QT_MOC_LITERAL(4, 49, 19), // "on_PBsetup_finished"
QT_MOC_LITERAL(5, 69, 6), // "result"
QT_MOC_LITERAL(6, 76, 13), // "focusOutEvent"
QT_MOC_LITERAL(7, 90, 12), // "QFocusEvent*"
QT_MOC_LITERAL(8, 103, 15) // "on_BytesWritten"

    },
    "PBsetup\0on_PBsetup_rejected\0\0"
    "on_PBsetup_accepted\0on_PBsetup_finished\0"
    "result\0focusOutEvent\0QFocusEvent*\0"
    "on_BytesWritten"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PBsetup[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    1,   41,    2, 0x08 /* Private */,
       6,    1,   44,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void,

       0        // eod
};

void PBsetup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PBsetup *_t = static_cast<PBsetup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_PBsetup_rejected(); break;
        case 1: _t->on_PBsetup_accepted(); break;
        case 2: _t->on_PBsetup_finished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->focusOutEvent((*reinterpret_cast< QFocusEvent*(*)>(_a[1]))); break;
        case 4: _t->on_BytesWritten(); break;
        default: ;
        }
    }
}

const QMetaObject PBsetup::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PBsetup.data,
      qt_meta_data_PBsetup,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *PBsetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PBsetup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PBsetup.stringdata0))
        return static_cast<void*>(const_cast< PBsetup*>(this));
    return QDialog::qt_metacast(_clname);
}

int PBsetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
