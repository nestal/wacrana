///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#include "MainWindow.hh"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtGlobal>
#include <iostream>

namespace WebHama {

MainWindow::MainWindow() : m_location{new QLineEdit{this}}
{
	m_ui.setupUi(this);
	m_ui.m_toolbar->addWidget(m_location);
	
	connect(m_location, &QLineEdit::returnPressed, this, &MainWindow::Go);
	
	// actions
	connect(m_ui.m_action_addtab, &QAction::triggered, this, &MainWindow::NewTab);
	connect(m_ui.m_action_back,   &QAction::triggered, this, &MainWindow::Back);
	
	// setup "new tab" button in the corner of the tab
	auto add_btn = new QToolButton{m_ui.m_tabs};
	add_btn->setDefaultAction(m_ui.m_action_addtab);
	m_ui.m_tabs->setCornerWidget(add_btn);
	
	// upload location bar when switching tabs
	connect(m_ui.m_tabs, &QTabWidget::currentChanged, [this](int tab)
	{
		Q_ASSERT(tab >= 0 && tab < m_ui.m_tabs->count());
		m_location->setText(Tab(tab)->Location().url());
	});
	
	// close tab when "x" button is pressed
	connect(m_ui.m_tabs, &QTabWidget::tabCloseRequested, [this](int tab)
	{
		if (m_ui.m_tabs->count() == 1)
			close();
		else
		{
			Tab(tab)->deleteLater();
			m_ui.m_tabs->removeTab(tab);
		}
	});
	
	// load home page
	NewTab()->Load({"https://google.com"});
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_last_cursor = event->globalPos() - frameGeometry().topLeft();
		event->accept();
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		window()->move(event->globalPos() - m_last_cursor);
		event->accept();
	}
}

BrowserTab* MainWindow::NewTab()
{
	auto tab = new BrowserTab{m_ui.m_tabs};
	connect(tab, &BrowserTab::LoadFinished, this, &MainWindow::OnLoad);
	connect(tab, &BrowserTab::IconChanged,  this, &MainWindow::OnIconChanged);
	m_ui.m_tabs->setCurrentIndex(m_ui.m_tabs->addTab(tab, tr("New Tab")));
	return tab;
}

void MainWindow::OnLoad(bool ok)
{
	auto tab = &dynamic_cast<BrowserTab&>(*sender());
	m_location->setText(tab->Location().url());
	m_ui.m_tabs->setTabText(IndexOf(tab), tab->Title());
	
	if (ok)
		statusBar()->hide();
}

void MainWindow::OnIconChanged(const QIcon& icon)
{
	auto tab = &dynamic_cast<BrowserTab&>(*sender());
	m_ui.m_tabs->setTabIcon(IndexOf(tab), tab->Icon());
}

void MainWindow::Go()
{
	QUrl url{m_location->text()};
	if (url.isRelative())
		url.setScheme("http");
		
	Current()->Load(url);
}

BrowserTab *MainWindow::Current()
{
	auto tab = m_ui.m_tabs->currentWidget();
	Q_ASSERT(tab);
	return &dynamic_cast<BrowserTab&>(*tab);
}

BrowserTab *MainWindow::Tab(int index)
{
	Q_ASSERT(index >= 0 && index < m_ui.m_tabs->count());
	auto tab = m_ui.m_tabs->widget(index);
	Q_ASSERT(tab);
	return &dynamic_cast<BrowserTab&>(*tab);
}

void MainWindow::Back()
{
	Current()->Back();
}

int MainWindow::IndexOf(V1::BrowserTab *tab) const
{
	return IndexOf(&dynamic_cast<BrowserTab&>(*tab));
}

int MainWindow::IndexOf(BrowserTab *tab) const
{
	auto index = m_ui.m_tabs->indexOf(tab);
	Q_ASSERT(index >= 0 && index < m_ui.m_tabs->count());
	return index;
}

} // end of namespace
