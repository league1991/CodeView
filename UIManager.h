#pragma once

namespace CodeAtlas
{

class UIManager:public QObject
{
	Q_OBJECT
public:
	UIManager(void);
	~UIManager(void);

	static UIManager* instance();
	static CodeScene& getScene();

	void			  addMainUI();
	void			  showAllUI()
	{
		for (int i = 0; i < m_mainUIList.size(); ++i)
		{
			m_mainUIList[i]->show();
		}
	}
	const QList<MainUI*>& getMainUIList(){return m_mainUIList;}

	void	   lockAll();
	void	   unlockAll();

public slots:
	void       updateScene();				// update scene when db updated
private:
	static UIManager* m_instance;

	QList<MainUI*>	  m_mainUIList;
	static CodeScene* m_scene;
};

}
