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
#include "BrowserTab.hh"

#include "ui_MainWindow.h"

class QLineEdit;

namespace WebHama {

class BrowserTab;

class MainWindow : public QMainWindow, public V1::MainWindow
{
	Q_OBJECT

public:
	MainWindow();

	BrowserTab* NewTab() override;
	
	BrowserTab* Current() override;
	BrowserTab* Tab(int index) override;
	
private:
	void OnLoad(bool);
	void OnIconChanged(const QIcon& icon);
	void Go();
	void Back();

	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	
private:
	Ui::MainWindow m_ui;
	QLineEdit *m_location{};
	QPoint m_last_cursor;
};

} // end of namespace
