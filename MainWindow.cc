/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////
#include "MainWindow.hh"

#include <QWebEngineView>

MainWindow::MainWindow() :
	m_page{new QWebEngineView{this}}
{
	m_page->load({"http://en.cppreference.com/"});
	setCentralWidget(m_page);

	connect(m_page, &QWebEngineView::loadFinished, this, &MainWindow::OnLoadFinished);
}

void MainWindow::OnLoadFinished(bool)
{
	setWindowIcon(m_page->icon());
}
