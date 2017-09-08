/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#include "MainWindow.hh"

#include <iostream>

MainWindow::MainWindow()
{
	m_ui.setupUi(this);
	
	connect(m_ui.m_page, &QWebEngineView::loadFinished, this, &MainWindow::OnLoad);
	
	m_ui.m_page->load({"https://google.com"});
}

void MainWindow::OnLoad(bool val)
{
	std::cout << "OnLoad() " << val << std::endl;
}
