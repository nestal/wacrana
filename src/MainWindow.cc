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

namespace WebHama {

MainWindow::MainWindow() : m_location{new QLineEdit{this}}
{
	m_ui.setupUi(this);
	m_ui.m_toolbar->addWidget(m_location);
	
	connect(m_location, &QLineEdit::returnPressed, this, &MainWindow::Go);
	
	// setup "new tab" button in the corner of the tab
	auto add_btn = new QToolButton{m_ui.m_tabs};
	add_btn->setDefaultAction(m_ui.m_action_addtab);
	m_ui.m_tabs->setCornerWidget(add_btn);
	connect(add_btn, &QToolButton::clicked, this, &MainWindow::NewTab);
	
	// upload location bar when switching tabs
	connect(m_ui.m_tabs, &QTabWidget::currentChanged, [this](int tab)
	{
		m_location->setText(Tab(tab)->Location().url());
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
	m_ui.m_tabs->setCurrentIndex(m_ui.m_tabs->addTab(tab, "New Tab"));
	return tab;
}

void MainWindow::OnLoad(bool)
{
	if (auto tab = dynamic_cast<BrowserTab*>(sender()))
	{
		auto index = m_ui.m_tabs->indexOf(tab);
		Q_ASSERT(index != -1);
		
		m_location->setText(tab->Location().url());
		m_ui.m_tabs->setTabText(index, tab->Title());
	}
}

void MainWindow::OnIconChanged(const QIcon& icon)
{
	if (auto tab = dynamic_cast<BrowserTab*>(sender()))
	{
		auto index = m_ui.m_tabs->indexOf(tab);
		Q_ASSERT(index != -1);
		
		m_ui.m_tabs->setTabIcon(index, tab->Icon());
	}
}

void MainWindow::Go()
{
	QUrl url{m_location->text()};
	if (url.isRelative())
	{
		url.setScheme("http");
		
		if (auto tab = Current())
			tab->Load(url);
	}
}

BrowserTab *MainWindow::Current()
{
	return dynamic_cast<BrowserTab*>(m_ui.m_tabs->currentWidget());
}

BrowserTab *MainWindow::Tab(int index)
{
	return dynamic_cast<BrowserTab*>(m_ui.m_tabs->widget(index));
}

} // end of namespace
