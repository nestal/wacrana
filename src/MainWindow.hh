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
#include "include/MainWindow.hpp"

// dependencies
#include "BrowserTab.hh"
#include "ui_MainWindow.h"

class QLineEdit;

namespace wacrana {

class BrowserTab;

class MainWindow : public QMainWindow, public V1::MainWindow
{
	Q_OBJECT

public:
	MainWindow();

	BrowserTab& NewTab() override;
	
	BrowserTab& Current() override;
	BrowserTab& Tab(int index) override;
	int IndexOf(const V1::BrowserTab& tab) const override;
	int Count() const override;
	
private:
	void OnLoad(bool);
	void OnIconChanged(const QIcon& icon);
	void Go();
	void Back();
	int IndexOf(const BrowserTab& tab) const ;
	
private:
	Ui::MainWindow m_ui;
	QLineEdit *m_location;
};

} // end of namespace
