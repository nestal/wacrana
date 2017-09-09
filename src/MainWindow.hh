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
#include "ui_MainWindow.h"
#include "Configuration.hh"

#include <memory>

class QLineEdit;
class QToolButton;

namespace wacrana {

class BrowserTab;

class MainWindow : public QMainWindow, public V1::MainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

	BrowserTab& NewTab() override;
	
	BrowserTab& Current() override;
	BrowserTab& Tab(int index) override;
	int IndexOf(const V1::BrowserTab& tab) const override;
	int Count() const override;
	
private:
	void Go();
	void Back();
	int IndexOf(const BrowserTab& tab) const ;
	void InitMenu();
	
private:
	Configuration  m_config;
	
	Ui::MainWindow m_ui;
	QLineEdit   *m_location{};
	QToolButton *m_menu_btn{};
};

} // end of namespace
