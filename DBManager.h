#pragma once

namespace CodeAtlas
{
class DBManager
{
public:
	DBManager(void);
	~DBManager(void);

	static DBManager*		instance(){return s_instance;}

	bool					openDB(char* fileName);
	bool					closeDB();
	UnderstandDB*			getDB(){return &m_db;}

	bool					parse();

	SymbolTreeBuilder*		getBuilder(){return &builder;}
	SymbolTree&				getSymbolTree();
private:
	static DBManager*		s_instance;

	UnderstandDB			m_db;
	SymbolTreeBuilder		builder;
};
}
