// CodeView.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
using namespace CodeAtlas;

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	new UIManager;
	new DBManager;

	DBManager* db = DBManager::instance();
	UIManager* ui = UIManager::instance();

	//db->openDB("GeometryProcess.udb");
	db->openDB("CodeView.udb");
	//db->openDB("qtcreator.udb");

	ui->lockAll();
	db->parse();
	ui->unlockAll();

	ui->updateScene();
	ui->addMainUI();
	//ui->addMainUI();
	ui->showAllUI();

	app.exec();
	DBManager::instance()->closeDB();

	return 0;
}

