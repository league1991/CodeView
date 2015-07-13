#pragma once

namespace CodeAtlas{

struct DBDependData
{
	DBDependData(const UnderstandHandle&src, const UnderstandHandle&tar, unsigned type):m_src(src), m_tar(tar), m_type(type){}
	friend uint qHash(const DBDependData& data);

	const UnderstandHandle& getSrc()const{return m_src;}
	const UnderstandHandle& getTar()const{return m_tar;}
	unsigned				getType()const{return m_type;}
private:
	UnderstandHandle m_src, m_tar;
	unsigned		 m_type;
};

uint qHash(const DBDependData& data)
{
	uint srcHash = qHash(data.m_src);
	uint tarHash = qHash(data.m_tar);
	return srcHash ^ tarHash ^ data.m_type;
}

class SymbolTreeBuilder
{
public:
	SymbolTreeBuilder(void);
	~SymbolTreeBuilder(void);

	void			setDababase(UnderstandDB* db){m_db = db;}
	UnderstandDB*   getDatabase(){return m_db;}

	bool			buildTree();
	SymbolTree&     getTree(){return m_tree;}

	void			deleteAllModidier(){m_modifierList.clear();}
	void			addModifier( const SymbolModifier::Ptr& modifier );
	void			clearModifierCache();
	void			applyModifiers();

	bool			getSymbolPath(const UnderstandHandle& hdl, SymbolPath& path);
private:
	static QString		commonPrefix(const QString& s1, const QString& s2);
	static void			findFolderPath(const QList<QString>&fp, QStringList& pathList);
	
	static void mergeNode( SymbolNode::Ptr& parent, const QSet<SymbolNode::Ptr>& leafSet);
	bool buildTreeRecursive(SymbolNode::Ptr& parent, SymbolPath& parentPath, UnderstandHandle& parentHdl );

	static SymbolInfo	combineFolderInfo(const SymbolInfo& i1, const SymbolInfo& i2)
	{
		return SymbolInfo(i1.name()+"\\"+i2.name(), "folder", SymbolInfo::Folder);
	}
	QList<SymbolModifier::Ptr> m_modifierList;

	//QList<DBDependData>		   m_dependData;
	QHash<UnderstandHandle, SymbolPath> m_handleMap;
	
	SymbolTree		m_tree;
	UnderstandDB*	m_db; 
};

}
