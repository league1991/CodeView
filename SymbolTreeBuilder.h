#pragma once

namespace CodeAtlas{

struct DBDependData
{
	DBDependData(const UnderstandHandle&src, const UnderstandHandle&tar):m_src(src), m_tar(tar){}
	friend uint qHash(const DBDependData& data);

	const UnderstandHandle& getSrc()const{return m_src;}
	const UnderstandHandle& getTar()const{return m_tar;}
private:
	UnderstandHandle m_src, m_tar;
};

uint qHash(const DBDependData& data)
{
	uint srcHash = qHash(data.m_src);
	uint tarHash = qHash(data.m_tar);
	return srcHash ^ tarHash;
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
	bool buildTreeRecursive(SymbolNode::Ptr& parent, SymbolPath& parentPath, UnderstandHandle& parentHdl );

	QList<SymbolModifier::Ptr> m_modifierList;

	//QList<DBDependData>		   m_dependData;
	QHash<UnderstandHandle, SymbolPath> m_handleMap;
	
	SymbolTree		m_tree;
	UnderstandDB*	m_db; 
};

}
