///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#include "MainWindow.hh"

#include "Configuration.hh"
#include "Plugin.hpp"
#include "ui_MainWindow.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QThread>

namespace wacrana {

MainWindow::MainWindow(Configuration& config) :
	m_config{config},
	m_ui{std::make_unique<Ui::MainWindow>()},
	m_location{new QLineEdit(this)}
{
	connect(&m_config, &Configuration::Finish, this, &MainWindow::OnConfigReady);
	
	m_ui->setupUi(this);
	m_ui->m_toolbar->addWidget(m_location);
	m_ui->m_toolbar->addSeparator();

	connect(m_location, &QLineEdit::returnPressed, this, &MainWindow::Go);
	
	// actions
	connect(m_ui->m_action_addtab,   &QAction::triggered, [this]{NewTab();});
	connect(m_ui->m_action_back,     &QAction::triggered, [this]{Current().Back();});
	connect(m_ui->m_action_forward,  &QAction::triggered, [this]{Current().Forward();});
	connect(m_ui->m_action_exit,     &QAction::triggered, [this]{close();});
	connect(m_ui->m_action_zoom_in,  &QAction::triggered, [this]{Current().ZoomFactor(Current().ZoomFactor() * 1.25);});
	connect(m_ui->m_action_zoom_out, &QAction::triggered, [this]{Current().ZoomFactor(Current().ZoomFactor() / 1.25);});
	connect(m_ui->m_action_reset_zoom, &QAction::triggered, [this]{Current().ZoomFactor(m_config.DefaultZoom());});
	connect(m_ui->m_action_about,    &QAction::triggered, qApp, &QApplication::aboutQt);
	connect(m_ui->m_action_home,     &QAction::triggered, [this]
	{
		m_config.HomePage()->OnAction(*this, {});
	});
	connect(m_ui->m_action_reload,   &QAction::triggered, [this]{Current().Reload();});

	// setup "new tab" button in the corner of the tab
	auto add_btn = std::make_unique<QToolButton>(m_ui->m_tabs);
	add_btn->setDefaultAction(m_ui->m_action_addtab);
	m_ui->m_tabs->setCornerWidget(add_btn.release());
	
	InitMenu();
	
	// upload location bar when switching tabs
	connect(m_ui->m_tabs, &QTabWidget::currentChanged, [this](int tab)
	{
		Q_ASSERT(tab >= 0 && tab < m_ui->m_tabs->count());
		auto&& browser = Tab(tab);
		SetLocation(browser.Location().url());
		setWindowTitle(browser.Title());
	});
	
	// close tab when "x" button is pressed
	connect(m_ui->m_tabs, &QTabWidget::tabCloseRequested, [this](int tab)
	{
		// close the main window when the last tab is closed
		if (m_ui->m_tabs->count() == 1)
			close();
		else
		{
			// the tab widget will not delete the tabs, so we need to delete them ourselves.
			auto widget = m_ui->m_tabs->widget(tab);
			m_ui->m_tabs->removeTab(tab);
			delete widget;
		}
	});
	
	// double click the tab bar will create new tab
	connect(m_ui->m_tabs->tabBar(), &QTabBar::tabBarDoubleClicked, [this](int tab)
	{
		if (tab == -1)
			NewTab();
	});
	
	// load home page
	NewTab();
	m_config.HomePage()->OnAction(*this, {});
}

MainWindow::~MainWindow() = default;

BrowserTab& MainWindow::NewTab()
{
	auto tab = new BrowserTab{m_ui->m_tabs, m_config.DefaultZoom()};
	connect(tab, &BrowserTab::LoadFinished, [this, tab](bool ok)
	{
		SetLocation(tab->Location().url());
		m_ui->m_tabs->setTabText(IndexOf(*tab), tab->Title());

		// need to reset zoom factor after loading a site
		tab->ZoomFactor(m_config.DefaultZoom());
		
		if (ok)
			statusBar()->hide();
	});
	connect(tab, &BrowserTab::IconChanged,  [this, tab](const QIcon& icon)
	{
		m_ui->m_tabs->setTabIcon(IndexOf(*tab), icon);
	});
	connect(tab, &BrowserTab::TitleChanged, [this, tab](const QString& title)
	{
		m_ui->m_tabs->setTabText(IndexOf(*tab), title);
	});
	m_ui->m_tabs->setCurrentIndex(m_ui->m_tabs->addTab(tab, tr("New Tab")));
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
	auto tab = m_ui->m_tabs->currentWidget();
	Q_ASSERT(tab);
	return *qobject_cast<BrowserTab*>(tab);
}

BrowserTab& MainWindow::Tab(int index)
{
	Q_ASSERT(index >= 0 && index < m_ui->m_tabs->count());
	auto tab = m_ui->m_tabs->widget(index);
	Q_ASSERT(tab);
	return *qobject_cast<BrowserTab*>(tab);
}

int MainWindow::IndexOf(const V1::BrowserTab& tab) const
{
	return IndexOf(dynamic_cast<const BrowserTab&>(tab));
}

int MainWindow::IndexOf(const BrowserTab& tab) const
{
	auto index = m_ui->m_tabs->indexOf(const_cast<BrowserTab*>(&tab));
	Q_ASSERT(index >= 0 && index < m_ui->m_tabs->count());
	return index;
}

int MainWindow::TabCount() const
{
	return m_ui->m_tabs->count();
}

void MainWindow::InitMenu()
{
	Q_ASSERT(!m_tab_menu);
	
	auto menu = std::make_unique<QMenu>();
	m_tab_menu = menu->addMenu(tr("New tab"));
		m_tab_menu->addAction(m_ui->m_action_addtab);
	menu->addAction(m_ui->m_action_zoom_in);
	menu->addAction(m_ui->m_action_zoom_out);
	menu->addAction(m_ui->m_action_reset_zoom);
	menu->addSeparator();
	menu->addAction(m_ui->m_action_about);
	menu->addSeparator();
	menu->addAction(m_ui->m_action_exit);
	
	m_menu_btn = new QToolButton;
	m_menu_btn->setIcon(QIcon{":/icon/ic_menu_black_24px.svg"});
	m_menu_btn->setMenu(menu.release());
	m_menu_btn->setPopupMode(QToolButton::InstantPopup);
	m_ui->m_toolbar->addWidget(m_menu_btn);
}

void MainWindow::OnConfigReady()
{
	Q_ASSERT(QThread::currentThread() == m_config.thread());
	try
	{
		m_config.GetResult();
		
		Q_ASSERT(m_tab_menu);
		
		for (auto&& persona : m_config.Persona())
		{
			qDebug() << "loaded persona: " << persona->Name();
			auto action = new QAction{persona->Name(), this};
			connect(action, &QAction::triggered, [this, &persona]
			{
				NewTab().SetPersona(*persona);
			});
			m_tab_menu->addAction(action);
		}
	}
	catch (std::exception& e)
	{
		QMessageBox::critical(this, QObject::tr("Configuration Error"), e.what());
	}
	catch (...)
	{
		QMessageBox::critical(this, QObject::tr("Configuration Error"), "Unknown exception");
	}
}

void MainWindow::SetLocation(const QString& loc)
{
	m_location->setText(loc == "about:blank" ? "" : loc);
}

} // end of namespace
