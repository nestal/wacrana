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

namespace WebHama {

MainWindow::MainWindow()
{
	m_ui.setupUi(this);
	
	connect(m_ui.m_page, &QWebEngineView::loadFinished, this, &MainWindow::OnLoad);
	connect(m_ui.m_location, &QLineEdit::returnPressed, this, &MainWindow::Go);
	
	m_ui.m_page->load({"https://google.com"});
}

void MainWindow::OnLoad(bool val)
{
	m_ui.m_location->setText(m_ui.m_page->url().url());
	setWindowTitle(m_ui.m_page->title() + " - webhama");
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

void MainWindow::Go()
{
	QUrl url{m_ui.m_location->text()};
	if (url.isRelative())
	{
		url.setScheme("http");
		m_ui.m_page->load(url);
	}
	
}

} // end of namespace
