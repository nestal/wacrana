///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#include "MainWindow.hh"

#include "Plugin.hpp"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include <QtCore/QLibrary>

#include <memory>

namespace wacrana {

MainWindow::MainWindow() :
	m_location{new QLineEdit(this)}
{
	m_ui.setupUi(this);
	m_ui.m_toolbar->addWidget(m_location);
	
	connect(m_location, &QLineEdit::returnPressed, this, &MainWindow::Go);
	
	// actions
	connect(m_ui.m_action_addtab,   &QAction::triggered, [this]{NewTab();});
	connect(m_ui.m_action_back,     &QAction::triggered, [this]{Back();});
	connect(m_ui.m_action_exit,     &QAction::triggered, [this]{close();});
	connect(m_ui.m_action_zoom_in,  &QAction::triggered, [this]{Current().ZoomIn();});
	connect(m_ui.m_action_zoom_out, &QAction::triggered, [this]{Current().ZoomOut();});
	connect(m_ui.m_action_home,     &QAction::triggered, [this]
	{
		m_config.HomePage()->OnAction(*this, {});
	});
	
	// Must connect the signal before calling Configuration::Load(), otherwise the signal
	// may be missed.
	connect(&m_config, &Configuration::Finish, this, [this]
	{
		try
		{
			m_config.GetResult();
		}
		catch (std::exception& e)
		{
			QMessageBox::critical(this, tr("Exception"), e.what());
		}
	}, Qt::QueuedConnection);
	m_config.Load("wacrana.json");
	
	// setup "new tab" button in the corner of the tab
	auto add_btn = std::make_unique<QToolButton>(m_ui.m_tabs);
	add_btn->setDefaultAction(m_ui.m_action_addtab);
	m_ui.m_tabs->setCornerWidget(add_btn.release());
	
	InitMenu();
	
	// upload location bar when switching tabs
	connect(m_ui.m_tabs, &QTabWidget::currentChanged, [this](int tab)
	{
		Q_ASSERT(tab >= 0 && tab < m_ui.m_tabs->count());
		m_location->setText(Tab(tab).Location().url());
	});
	
	// close tab when "x" button is pressed
	connect(m_ui.m_tabs, &QTabWidget::tabCloseRequested, [this](int tab)
	{
		// close the main window when the last tab is closed
		if (m_ui.m_tabs->count() == 1)
			close();
		else
		{
			Tab(tab).deleteLater();
			m_ui.m_tabs->removeTab(tab);
		}
	});
	
	// load home page
	NewTab();
	m_config.HomePage()->OnAction(*this, {});
}

MainWindow::~MainWindow() = default;

BrowserTab& MainWindow::NewTab()
{
	auto tab = new BrowserTab{m_ui.m_tabs, m_config.DefaultZoom()};
	connect(tab, &BrowserTab::LoadFinished, [this, tab](bool ok)
	{
		m_location->setText(tab->Location().url());
		m_ui.m_tabs->setTabText(IndexOf(*tab), tab->Title());

		// need to reset zoom factor after loading a site
		tab->ZoomFactor(m_config.DefaultZoom());
		
		if (ok)
			statusBar()->hide();
	});
	connect(tab, &BrowserTab::IconChanged,  [this, tab](const QIcon& icon)
	{
		m_ui.m_tabs->setTabIcon(IndexOf(*tab), icon);
	});
	connect(tab, &BrowserTab::TitleChanged, [this, tab](const QString& title)
	{
		m_ui.m_tabs->setTabText(IndexOf(*tab), title);
	});
	m_ui.m_tabs->setCurrentIndex(m_ui.m_tabs->addTab(tab, tr("New Tab")));
	return *tab;
}

void MainWindow::Go()
{
	QUrl url{m_location->text()};
	if (url.isRelative())
		url.setScheme("http");
		
	Current().Load(url);
}

BrowserTab& MainWindow::Current()
{
	auto tab = m_ui.m_tabs->currentWidget();
	Q_ASSERT(tab);
	return dynamic_cast<BrowserTab&>(*tab);
}

BrowserTab& MainWindow::Tab(int index)
{
	Q_ASSERT(index >= 0 && index < m_ui.m_tabs->count());
	auto tab = m_ui.m_tabs->widget(index);
	Q_ASSERT(tab);
	return dynamic_cast<BrowserTab&>(*tab);
}

void MainWindow::Back()
{
	Current().Back();
}

int MainWindow::IndexOf(const V1::BrowserTab& tab) const
{
	return IndexOf(dynamic_cast<const BrowserTab&>(tab));
}

int MainWindow::IndexOf(const BrowserTab& tab) const
{
	auto index = m_ui.m_tabs->indexOf(const_cast<BrowserTab*>(&tab));
	Q_ASSERT(index >= 0 && index < m_ui.m_tabs->count());
	return index;
}

int MainWindow::Count() const
{
	return m_ui.m_tabs->count();
}

void MainWindow::InitMenu()
{
	auto menu = std::make_unique<QMenu>();
	menu->addAction(m_ui.m_action_addtab);
	menu->addAction(m_ui.m_action_zoom_in);
	menu->addAction(m_ui.m_action_zoom_out);
	menu->addSeparator();
	menu->addAction(m_ui.m_action_exit);
	
	auto button = std::make_unique<QToolButton>();
	button->setIcon(QIcon{":/icon/ic_menu_black_24px.svg"});
	button->setMenu(menu.release());
	button->setPopupMode(QToolButton::InstantPopup);
	m_ui.m_toolbar->addWidget(button.release());
}

} // end of namespace
