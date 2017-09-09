/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the webhama
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#pragma once

#include <QtWidgets/QWidget>
#include "include/BrowserTab.hpp"

#include "ui_BrowserTab.h"

class QUrl;

namespace WebHama {

class BrowserTab : public QWidget, public V1::BrowserTab
{
	Q_OBJECT

public :
	BrowserTab(QWidget *parent);

	void Load(const QUrl& url) override;
	QUrl Location() const override;
	QString Title() const override;
	QIcon Icon() const override;
	
Q_SIGNALS:
	void LoadFinished(bool);
	void IconChanged(const QIcon& icon);
	
private:
	void OnLoad(bool);
	
private:
	Ui::BrowserTab m_ui;
};

} // end of namespace
