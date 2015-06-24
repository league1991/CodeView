/****************************************************************************
** Meta object code from reading C++ file 'DBManager.h'
**
** Created: Tue May 12 18:04:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#include "../../shared/qtcreator_gui_pch.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DBManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CodeAtlas__DBManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   22,   21,   21, 0x05,
      54,   47,   21,   21, 0x05,
     100,   21,   21,   21, 0x05,
     124,   21,   21,   21, 0x05,
     151,   21,   21,   21, 0x05,
     171,   21,   21,   21, 0x05,
     199,  194,   21,   21, 0x05,
     224,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
     257,  236,   21,   21, 0x0a,
     301,  287,   21,   21, 0x2a,
     336,  327,   21,   21, 0x2a,
     368,  358,   21,   21, 0x0a,
     399,  358,   21,   21, 0x0a,
     435,   21,   21,   21, 0x0a,
     461,  194,   21,   21, 0x0a,
     479,   21,   21,   21, 0x09,
     510,  499,   21,   21, 0x09,
     544,   22,   21,   21, 0x09,
     565,   21,   21,   21, 0x09,
     588,   47,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CodeAtlas__DBManager[] = {
    "CodeAtlas::DBManager\0\0state\0"
    "stateChanged(bool)\0result\0"
    "treeDataUpdate(QSharedPointer<QStandardItem>)\0"
    "requestTreeDataUpdate()\0"
    "requestResetCurrentState()\0"
    "requestClearCache()\0requestClearCacheAll()\0"
    "flat\0requestSetFlatMode(bool)\0dbUpdated()\0"
    "fileName,line,column\0gotoLocation(QString,int,int)\0"
    "fileName,line\0gotoLocation(QString,int)\0"
    "fileName\0gotoLocation(QString)\0locations\0"
    "gotoLocations(QList<QVariant>)\0"
    "gotoLocations(QSet<SymbolLocation>)\0"
    "onRequestTreeDataUpdate()\0setFlatMode(bool)\0"
    "onWidgetIsCreated()\0visibility\0"
    "onWidgetVisibilityIsChanged(bool)\0"
    "onStateChanged(bool)\0onProjectListChanged()\0"
    "onTreeDataUpdate(QSharedPointer<QStandardItem>)\0"
};

void CodeAtlas::DBManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DBManager *_t = static_cast<DBManager *>(_o);
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->treeDataUpdate((*reinterpret_cast< QSharedPointer<QStandardItem>(*)>(_a[1]))); break;
        case 2: _t->requestTreeDataUpdate(); break;
        case 3: _t->requestResetCurrentState(); break;
        case 4: _t->requestClearCache(); break;
        case 5: _t->requestClearCacheAll(); break;
        case 6: _t->requestSetFlatMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->dbUpdated(); break;
        case 8: _t->gotoLocation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 9: _t->gotoLocation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->gotoLocation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->gotoLocations((*reinterpret_cast< const QList<QVariant>(*)>(_a[1]))); break;
        case 12: _t->gotoLocations((*reinterpret_cast< const QSet<SymbolLocation>(*)>(_a[1]))); break;
        case 13: _t->onRequestTreeDataUpdate(); break;
        case 14: _t->setFlatMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->onWidgetIsCreated(); break;
        case 16: _t->onWidgetVisibilityIsChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->onStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->onProjectListChanged(); break;
        case 19: _t->onTreeDataUpdate((*reinterpret_cast< QSharedPointer<QStandardItem>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CodeAtlas::DBManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CodeAtlas::DBManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CodeAtlas__DBManager,
      qt_meta_data_CodeAtlas__DBManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CodeAtlas::DBManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CodeAtlas::DBManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CodeAtlas::DBManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CodeAtlas__DBManager))
        return static_cast<void*>(const_cast< DBManager*>(this));
    return QObject::qt_metacast(_clname);
}

int CodeAtlas::DBManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void CodeAtlas::DBManager::stateChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CodeAtlas::DBManager::treeDataUpdate(QSharedPointer<QStandardItem> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CodeAtlas::DBManager::requestTreeDataUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CodeAtlas::DBManager::requestResetCurrentState()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void CodeAtlas::DBManager::requestClearCache()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void CodeAtlas::DBManager::requestClearCacheAll()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void CodeAtlas::DBManager::requestSetFlatMode(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CodeAtlas::DBManager::dbUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}
QT_END_MOC_NAMESPACE
