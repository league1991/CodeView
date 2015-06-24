#include "StdAfx.h"
#include "UIManager.h"

using namespace CodeAtlas;

CodeScene* UIManager::m_scene		= NULL;
UIManager* UIManager::m_instance	= NULL;

UIManager::UIManager(void)
{
	m_instance = this;
}

UIManager::~UIManager(void)
{
	m_instance = NULL;
	delete m_scene;
	for (int i = 0; i < m_mainUIList.size(); ++i)
	{
		delete m_mainUIList[i];
	}
	m_mainUIList.clear();
}

UIManager* UIManager::instance()
{
	return m_instance;
}


void CodeAtlas::UIManager::updateScene()
{
	//Locator& locator = DBManager::instance()->getDB().getVocabularyCollector();
	//m_scene.updateSceneData(locator.getProjectData());
	getScene().update();
}

void CodeAtlas::UIManager::lockAll()
{
	getScene().lockScene();
	for (int i = 0; i < m_mainUIList.size(); ++i)
	{
		m_mainUIList[i]->lockUI();
	}
}

void CodeAtlas::UIManager::unlockAll()
{
	for (int i = 0; i < m_mainUIList.size(); ++i)
	{
		m_mainUIList[i]->unlockUI();
	}
	getScene().unlockScene();
}

CodeScene& CodeAtlas::UIManager::getScene()
{
	if (m_scene == NULL)
	{
		QGraphicsScene s;
		m_scene = new CodeScene;
		m_scene->init();
	}
	return *m_scene;
}

void CodeAtlas::UIManager::addMainUI()
{
	m_mainUIList.push_back(new MainUI());
}
