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
	QList<SymbolNode::Ptr>	globalNodes;
	m_db->findGlobalSymbols(globalHandle);
	bool res = true;
	SymbolNode::Ptr root(new SymbolNode());
	QSet<SymbolNode::Ptr> leafSet;

	// find folder nodes
	for (int i = 0; i < globalHandle.size(); ++i)
	{
		// find file path
		QList<QString> fp;
		m_db->findFilePaths(globalHandle[i],fp);
		QStringList pathList;
		findFolderPath(fp, pathList);
		SymbolNode::Ptr curNode = root;
		for (int p = 0; p < pathList.size(); ++p)
		{
			SymbolInfo folderInfo(pathList[p], "folder", SymbolInfo::Folder);
			curNode = SymbolNode::addOrFindChild(curNode, folderInfo,0);
		}
		globalNodes.append(curNode);
		leafSet.insert(curNode);
	}

	root->printTree(0);
	// merge useless folder nodes
	mergeNode(root, leafSet);
	root->printTree(0);

	for (int i = 0; i < globalHandle.size(); ++i)
	{
		SymbolNode::Ptr& curNode = globalNodes[i];

		SymbolInfo info = m_db->getSymbolInfo(globalHandle[i]);
		info.setTopLevel(true);
		SymbolNode::Ptr sym = SymbolNode::addOrFindChild(curNode, info,0);
		
		SymbolPath path = sym->getSymPath();
		qDebug() << path.toString();
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
	if (!parentInfo)
	{
		return true;
	}

	// add children
	// for functions, only accept parameters
	char* childKind = (parentInfo->elementType() & SymbolInfo::FunctionSignalSlot) ? "parameter" : "~file";
	QList<UnderstandHandle> childHdl;
	m_db->findChildren(parentHdl, childHdl, childKind);
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

QString CodeAtlas::SymbolTreeBuilder::commonPrefix( const QString& s1, const QString& s2 )
{
	int end = min(s1.length(), s2.length());
	const QChar* ps1 = s1.constData();
	const QChar* ps2 = s2.constData();
	int length = 0;
	for (; length < end; ++length, ++ps1, ++ps2)
	{
		if (*ps1 != *ps2)
		{
			break;
		}
	}
	return s1.left(length);
}

void CodeAtlas::SymbolTreeBuilder::findFolderPath( const QList<QString>&fileList, QStringList& pathList )
{
	const QChar backslash('\\');
	QString pathPrefix;
	if (fileList.size()  != 0)
	{
		pathPrefix = fileList[0];
		
		for (int i = 1; i < fileList.size(); ++i)
		{
			pathPrefix = commonPrefix(pathPrefix, fileList[i]);
		}
	}

	
	// remove fileName
	int fileNamePos = pathPrefix.lastIndexOf(backslash);
	if (fileNamePos != -1)
	{
		pathPrefix = pathPrefix.left(fileNamePos);
	}

	// split paths, and construct directory structures
	pathList = pathPrefix.split(backslash);
}
void CodeAtlas::SymbolTreeBuilder::mergeNode( SymbolNode::Ptr& parent, const QSet<SymbolNode::Ptr>& leafSet)
{
	if (parent->childCount() == 1)
	{
		SymbolNode::Ptr child = parent->childBegin().value();

		// ignore leaf
		if (child->childCount() != 0 && !leafSet.contains(child))
		{
			// merge child and descendents
			SymbolInfo   childInfo = parent->childBegin().key();
			parent->removeChild(childInfo);

			for (SymbolNode::ChildIterator pDesc = child->childBegin(); pDesc != child->childEnd(); ++pDesc)
			{
				SymbolInfo newInfo   = combineFolderInfo(childInfo, pDesc.key());
				SymbolNode::addOrReplaceChild(parent, newInfo, pDesc.value());
			}
		}
	}
	for (SymbolNode::ChildIterator pChild = parent->childBegin(); pChild != parent->childEnd(); ++pChild)
	{
		mergeNode(pChild.value(), leafSet);
	}
}

