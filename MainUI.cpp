#include "StdAfx.h"
#include "MainUI.h"
using namespace CodeAtlas;


MainUI::MainUI(void)
{
	m_ui.setupUi(this);

	buildConnections();
	//setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

MainUI::~MainUI(void)
{
}


void CodeAtlas::MainUI::buildConnections()
{
	connect(m_ui.actionShowFunctions, SIGNAL(toggled(bool)), this, SLOT(functionCheck(bool)));
	connect(m_ui.actionShowVariables, SIGNAL(toggled(bool)), this, SLOT(variableCheck(bool)));
	connect(m_ui.actionShowNames, SIGNAL(toggled(bool)), this, SLOT(nameCheck(bool)));
	connect(m_ui.actionShowWordCloud, SIGNAL(toggled(bool)), this, SLOT(wordCloudCheck(bool)));
	connect(m_ui.actionShowModuleCluster, SIGNAL(toggled(bool)), this, SLOT(moduleClusterCheck(bool)));
}

void CodeAtlas::MainUI::resetDB()
{

}

void CodeAtlas::MainUI::onActivate( bool isActivate )
{
}

void CodeAtlas::MainUI::unlockUI()
{
	CodeView* view = (CodeView*)m_ui.centralwidget;
	view->unlockView();
}

void CodeAtlas::MainUI::lockUI()
{
	CodeView* view = (CodeView*)m_ui.centralwidget;
	view->lockView();
}

void CodeAtlas::MainUI::functionCheck( bool checked )
{
	unsigned filter = getView()->getLodManager().getSymFilter();
	if (checked)
		getView()->getLodManager().setSymFilter(filter | SymbolInfo::FunctionSignalSlot);
	else
		getView()->getLodManager().setSymFilter(filter & ~SymbolInfo::FunctionSignalSlot);
}

void CodeAtlas::MainUI::variableCheck( bool checked )
{
	unsigned filter = getView()->getLodManager().getSymFilter();
	if (checked)
		getView()->getLodManager().setSymFilter(filter | SymbolInfo::Variable);
	else
		getView()->getLodManager().setSymFilter(filter & ~SymbolInfo::Variable);
}

void CodeAtlas::MainUI::wordCloudCheck( bool checked )
{
	getView()->setDrawWordCloud(checked);
	getView()->update();
}

void CodeAtlas::MainUI::nameCheck( bool checked )
{
	getView()->setDrawNames(checked);
	getView()->update();
}

void CodeAtlas::MainUI::moduleClusterCheck( bool checked )
{
	getView()->showModuleCluster() = checked;
}
