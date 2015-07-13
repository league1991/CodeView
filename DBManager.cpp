#include "StdAfx.h"
#include "DBManager.h"

using namespace CodeAtlas;

DBManager* DBManager::s_instance = NULL;

DBManager::DBManager(void)
{
	s_instance = this;
}

DBManager::~DBManager(void)
{
}


bool CodeAtlas::DBManager::parse()
{
	CodeAtlas::UnderstandDB* udb = &m_db;

	builder.setDababase(udb);

	builder.addModifier(SymbolModifier::Ptr(new WordCollector));
	//builder.addModifier(SymbolModifier::Ptr(new FuzzyDependBuilder));
	builder.addModifier(SymbolModifier::Ptr(new GeneralDependBuilder));
	//builder.addModifier(SymbolModifier::Ptr(new Modulizer));
	builder.addModifier(SymbolModifier::Ptr(new UIElementLocator));

	builder.buildTree();
	builder.applyModifiers();
	builder.getTree().print();
	return true;
}

SymbolTree& CodeAtlas::DBManager::getSymbolTree()
{
	return builder.getTree();
}

bool CodeAtlas::DBManager::openDB( char* fileName )
{
	return m_db.open(fileName);
}

bool CodeAtlas::DBManager::closeDB()
{
	return m_db.close();
}
