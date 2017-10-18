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
#include "PluginManager.hh"
#include "BrightFuture/BrightFuture.hh"

#include <unordered_set>
#include <memory>
#include <random>

class QLineEdit;
class QToolButton;
class QProgressBar;

namespace wacrana {

class BrowserTab;
class Context;
class PluginManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public V1::MainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(Context& ctx);
	~MainWindow() override;

	BrowserTab& NewTab() override;
	
	BrowserTab& Current() override;
	BrowserTab& Tab(int index) override;
	int IndexOf(const V1::BrowserTab& tab) const override;
	int TabCount() const override;
	
private:
	void Go();
	int IndexOf(const BrowserTab& tab) const ;
	void InitMenu();
	void OnConfigReady(BrightFuture::future<PluginManager>&& future);
	void SetLocation(const QString& loc);
	std::shared_ptr<BrowserTab> RemoveTab(int tab);
	
private:
	Context&        m_ctx;
	PluginManager   m_plugins;      // Must put it before all other plugins

	std::unique_ptr<Ui::MainWindow> m_ui;
	std::unordered_set<std::shared_ptr<BrowserTab>> m_tabs; // must put after m_ui otherwise destructor will crash
	QLineEdit   *m_location{};
	QToolButton *m_menu_btn{};
	QMenu       *m_tab_menu{};
	QProgressBar *m_timer_progress{};
	
	V1::PluginPtr   m_home_page;
};

} // end of namespace
