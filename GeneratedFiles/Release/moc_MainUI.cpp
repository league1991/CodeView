/****************************************************************************
** Meta object code from reading C++ file 'MainUI.h'
**
** Created: Fri Jul 10 19:50:41 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CodeAtlas__MainUI[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,
      40,   29,   18,   18, 0x0a,
      65,   57,   18,   18, 0x0a,
      85,   57,   18,   18, 0x0a,
     105,   57,   18,   18, 0x0a,
     121,   57,   18,   18, 0x0a,
     142,   57,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CodeAtlas__MainUI[] = {
    "CodeAtlas::MainUI\0\0resetDB()\0isActivate\0"
    "onActivate(bool)\0checked\0functionCheck(bool)\0"
    "variableCheck(bool)\0nameCheck(bool)\0"
    "wordCloudCheck(bool)\0moduleClusterCheck(bool)\0"
};

void CodeAtlas::MainUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainUI *_t = static_cast<MainUI *>(_o);
        switch (_id) {
        case 0: _t->resetDB(); break;
        case 1: _t->onActivate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->functionCheck((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->variableCheck((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->nameCheck((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->wordCloudCheck((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->moduleClusterCheck((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CodeAtlas::MainUI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CodeAtlas::MainUI::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CodeAtlas__MainUI,
      qt_meta_data_CodeAtlas__MainUI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CodeAtlas::MainUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CodeAtlas::MainUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CodeAtlas::MainUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CodeAtlas__MainUI))
        return static_cast<void*>(const_cast< MainUI*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CodeAtlas::MainUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
