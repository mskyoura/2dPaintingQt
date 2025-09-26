/****************************************************************************
** Meta object code from reading C++ file 'appset.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../appset.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appset.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Appset_t {
    QByteArrayData data[11];
    char stringdata0[167];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Appset_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Appset_t qt_meta_stringdata_Appset = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Appset"
QT_MOC_LITERAL(1, 7, 18), // "on_eID_textChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 4), // "arg1"
QT_MOC_LITERAL(4, 32, 18), // "on_eT1_textChanged"
QT_MOC_LITERAL(5, 51, 18), // "on_eT2_textChanged"
QT_MOC_LITERAL(6, 70, 18), // "on_eU1_textChanged"
QT_MOC_LITERAL(7, 89, 18), // "on_eU2_textChanged"
QT_MOC_LITERAL(8, 108, 32), // "on_ePolarity_currentIndexChanged"
QT_MOC_LITERAL(9, 141, 5), // "index"
QT_MOC_LITERAL(10, 147, 19) // "on_eDst_textChanged"

    },
    "Appset\0on_eID_textChanged\0\0arg1\0"
    "on_eT1_textChanged\0on_eT2_textChanged\0"
    "on_eU1_textChanged\0on_eU2_textChanged\0"
    "on_ePolarity_currentIndexChanged\0index\0"
    "on_eDst_textChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Appset[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    1,   52,    2, 0x08 /* Private */,
       5,    1,   55,    2, 0x08 /* Private */,
       6,    1,   58,    2, 0x08 /* Private */,
       7,    1,   61,    2, 0x08 /* Private */,
       8,    1,   64,    2, 0x08 /* Private */,
      10,    1,   67,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Appset::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Appset *_t = static_cast<Appset *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_eID_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->on_eT1_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->on_eT2_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->on_eU1_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->on_eU2_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->on_ePolarity_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_eDst_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Appset::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Appset.data,
      qt_meta_data_Appset,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Appset::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Appset::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Appset.stringdata0))
        return static_cast<void*>(const_cast< Appset*>(this));
    return QDialog::qt_metacast(_clname);
}

int Appset::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
