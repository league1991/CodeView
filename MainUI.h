#pragma once
#include "GeneratedFiles/ui_mainUI.h"
namespace CodeAtlas
{

class MainUI:public QMainWindow
{
	Q_OBJECT
public:
	MainUI(void);
	~MainUI(void);

	void   lockUI();
	void   unlockUI();
public slots:
	void   resetDB();
	void   onActivate(bool isActivate);

	void   functionCheck(bool checked);
	void   variableCheck(bool checked);
	void   nameCheck(bool checked);
	void   wordCloudCheck(bool checked);
private:

	CodeView*		    getView()
	{
		return (CodeView*)m_ui.centralwidget;
	}
	void buildConnections();

	Ui::MainUI			m_ui;

};

}
