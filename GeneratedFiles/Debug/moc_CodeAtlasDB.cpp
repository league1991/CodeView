/****************************************************************************
** Meta object code from reading C++ file 'CodeAtlasDB.h'
**
** Created: Tue May 12 18:00:41 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#include "../../shared/qtcreator_gui_pch.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CodeAtlasDB.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CodeAtlas__CodeAtlasDB[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   23,   23,   23, 0x0a,
      52,   23,   23,   23, 0x0a,
      65,   23,   23,   23, 0x0a,
      76,   23,   23,   23, 0x0a,
     100,   91,   23,   23, 0x0a,
     125,   91,   23,   23, 0x0a,
     150,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CodeAtlas__CodeAtlasDB[] = {
    "CodeAtlas::CodeAtlasDB\0\0dbUpdated()\0"
    "clearCacheAll()\0clearCache()\0updateDB()\0"
    "updateCursor()\0fileList\0"
    "setFileList(QStringList)\0"
    "removeFiles(QStringList)\0"
    "resetDataToCurrentState()\0"
};

void CodeAtlas::CodeAtlasDB::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CodeAtlasDB *_t = static_cast<CodeAtlasDB *>(_o);
        switch (_id) {
        case 0: _t->dbUpdated(); break;
        case 1: _t->clearCacheAll(); break;
        case 2: _t->clearCache(); break;
        case 3: _t->updateDB(); break;
        case 4: _t->updateCursor(); break;
        case 5: _t->setFileList((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 6: _t->removeFiles((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 7: _t->resetDataToCurrentState(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CodeAtlas::CodeAtlasDB::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CodeAtlas::CodeAtlasDB::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CodeAtlas__CodeAtlasDB,
      qt_meta_data_CodeAtlas__CodeAtlasDB, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CodeAtlas::CodeAtlasDB::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CodeAtlas::CodeAtlasDB::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CodeAtlas::CodeAtlasDB::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CodeAtlas__CodeAtlasDB))
        return static_cast<void*>(const_cast< CodeAtlasDB*>(this));
    return QObject::qt_metacast(_clname);
}

int CodeAtlas::CodeAtlasDB::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void CodeAtlas::CodeAtlasDB::dbUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
