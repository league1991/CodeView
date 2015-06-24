/****************************************************************************
** Meta object code from reading C++ file 'SymbolTreeBuilder.h'
**
** Created: Tue May 12 17:44:39 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#include "../../shared/qtcreator_gui_pch.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SymbolTreeBuilder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CodeAtlas__SymbolTreeBuilder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x05,
      48,   29,   29,   29, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   29,   29,   29, 0x0a,
      80,   29,   29,   29, 0x0a,
      93,   29,   29,   29, 0x0a,
     111,  102,   29,   29, 0x0a,
     136,  102,   29,   29, 0x0a,
     170,  161,   29,   29, 0x0a,
     201,   29,   29,   29, 0x0a,
     231,  227,   29,   29, 0x0a,
     282,  277,   29,   29, 0x0a,
     300,   29,   29,   29, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CodeAtlas__SymbolTreeBuilder[] = {
    "CodeAtlas::SymbolTreeBuilder\0\0"
    "filesAreRemoved()\0resetDataDone()\0"
    "clearCacheAll()\0clearCache()\0update()\0"
    "fileList\0setFileList(QStringList)\0"
    "removeFiles(QStringList)\0snapshot\0"
    "resetData(CPlusPlus::Snapshot)\0"
    "resetDataToCurrentState()\0doc\0"
    "updateDocumentCache(CPlusPlus::Document::Ptr)\0"
    "flat\0setFlatMode(bool)\0onResetDataDone()\0"
};

void CodeAtlas::SymbolTreeBuilder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SymbolTreeBuilder *_t = static_cast<SymbolTreeBuilder *>(_o);
        switch (_id) {
        case 0: _t->filesAreRemoved(); break;
        case 1: _t->resetDataDone(); break;
        case 2: _t->clearCacheAll(); break;
        case 3: _t->clearCache(); break;
        case 4: _t->update(); break;
        case 5: _t->setFileList((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 6: _t->removeFiles((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 7: _t->resetData((*reinterpret_cast< const CPlusPlus::Snapshot(*)>(_a[1]))); break;
        case 8: _t->resetDataToCurrentState(); break;
        case 9: _t->updateDocumentCache((*reinterpret_cast< const CPlusPlus::Document::Ptr(*)>(_a[1]))); break;
        case 10: _t->setFlatMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->onResetDataDone(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CodeAtlas::SymbolTreeBuilder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CodeAtlas::SymbolTreeBuilder::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CodeAtlas__SymbolTreeBuilder,
      qt_meta_data_CodeAtlas__SymbolTreeBuilder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CodeAtlas::SymbolTreeBuilder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CodeAtlas::SymbolTreeBuilder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CodeAtlas::SymbolTreeBuilder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CodeAtlas__SymbolTreeBuilder))
        return static_cast<void*>(const_cast< SymbolTreeBuilder*>(this));
    return QObject::qt_metacast(_clname);
}

int CodeAtlas::SymbolTreeBuilder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void CodeAtlas::SymbolTreeBuilder::filesAreRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CodeAtlas::SymbolTreeBuilder::resetDataDone()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
