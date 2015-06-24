#include "StdAfx.h"
#include "SymbolTreeBuilder.h"

using namespace CodeAtlas;

SymbolTreeBuilder::SymbolTreeBuilder(void)
{
}

SymbolTreeBuilder::~SymbolTreeBuilder(void)
{
}

bool CodeAtlas::SymbolTreeBuilder::buildTree()
{
	if (!m_db)
	{
		return false;
	}
	QList<UnderstandHandle> globalHandle;
	m_db->findGlobalSymbols(globalHandle);
	bool res = true;
	SymbolNode::Ptr root(new SymbolNode());
	for (int i = 0; i < globalHandle.size(); ++i)
	{
		SymbolInfo info = m_db->getSymbolInfo(globalHandle[i]);
		SymbolNode::Ptr sym = SymbolNode::addOrFindChild(root, info,0);

		SymbolPath path;
		path.addChildSymbol(info);
		res &= buildTreeRecursive(sym, path, globalHandle[i]);
	}

	if (res)
	{
		m_tree = SymbolTree(root);
	}
	return res;
}

bool CodeAtlas::SymbolTreeBuilder::buildTreeRecursive(SymbolNode::Ptr& parent, 
													  SymbolPath& parentPath,
													  UnderstandHandle& parentHdl )
{
	// process parent
	m_handleMap[parentHdl] = parentPath;

	const SymbolInfo* parentInfo = parentPath.getLastSymbol();
	if (!parentInfo || (parentInfo->elementType() & SymbolInfo::FunctionSignalSlot))
	{
		return true;
	}

	// add children
	QList<UnderstandHandle> childHdl;
	m_db->findChildren(parentHdl, childHdl);
	bool res = true;
	for (int i = 0; i < childHdl.size(); ++i)
	{
		SymbolInfo childInfo = m_db->getSymbolInfo(childHdl[i]);
		SymbolNode::Ptr child = SymbolNode::addOrFindChild(parent, childInfo,0);
		child->setDBHandle(childHdl[i]);

		SymbolPath childPath = parentPath;
		childPath.addChildSymbol(childInfo);
		res &= buildTreeRecursive(child, childPath, childHdl[i]);
	}
	return res;
}

void CodeAtlas::SymbolTreeBuilder::addModifier( const SymbolModifier::Ptr& modifier )
{
	m_modifierList.push_back(modifier);
	modifier->setBuilder(this);
}

void CodeAtlas::SymbolTreeBuilder::applyModifiers()
{
	// modify tree
	for (int ithModifier = 0; ithModifier < m_modifierList.size(); ++ithModifier)
	{
		QTime t0 = QTime::currentTime();
		qDebug("################ begin apply modifier ################");
		m_modifierList[ithModifier]->prepare(m_tree);
		m_modifierList[ithModifier]->modifyTree();
		m_modifierList[ithModifier]->updateDirtyData();
		QTime t1 = QTime::currentTime();
		qDebug("modify time: %d", t0.msecsTo(t1));
		qDebug("################  end apply modifier  ################");
	}
}

void CodeAtlas::SymbolTreeBuilder::clearModifierCache()
{
	for (int ithModifier = 0; ithModifier < m_modifierList.size(); ++ithModifier)
	{
		m_modifierList[ithModifier]->clearCache();
	}
}

bool CodeAtlas::SymbolTreeBuilder::getSymbolPath( const UnderstandHandle& hdl, SymbolPath& path )
{
	QHash<UnderstandHandle, SymbolPath>::const_iterator p = m_handleMap.find(hdl);
	if (p == m_handleMap.constEnd())
	{
		return false;
	}
	path = p.value();
	return true;
}
