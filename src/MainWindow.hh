/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <QMainWindow>
#include "MainWindow.hpp"

// dependencies
#include "BrowserTab.hh"

#include <memory>

class QLineEdit;
class QToolButton;

namespace wacrana {

class BrowserTab;
class Configuration;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public V1::MainWindow
{
	Q_OBJECT

public:
	MainWindow(Configuration& config);
	~MainWindow();

	BrowserTab& NewTab() override;
	
	BrowserTab& Current() override;
	BrowserTab& Tab(int index) override;
	int IndexOf(const V1::BrowserTab& tab) const override;
	int TabCount() const override;
	
private:
	void Go();
	int IndexOf(const BrowserTab& tab) const ;
	void InitMenu();
	void OnConfigReady();
	void SetLocation(const QString& loc);
	
private:
	Configuration&  m_config;
	
	std::unique_ptr<Ui::MainWindow> m_ui;
	QLineEdit   *m_location{};
	QToolButton *m_menu_btn{};
	QMenu       *m_tab_menu{};
};

} // end of namespace
