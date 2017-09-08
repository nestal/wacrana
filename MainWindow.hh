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

class QWebEngineView;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

private:
	void OnLoadFinished(bool);

private:
	QWebEngineView *m_page{};
};
