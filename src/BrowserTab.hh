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
#include <memory>

class QUrl;
class QWebEnginePage;

namespace wacrana {

namespace V1 {
class Plugin;
}

class BrowserTab : public QWidget, public V1::BrowserTab
{
	Q_OBJECT

public :
	explicit BrowserTab(QWidget *parent, double zoom);
	BrowserTab(const BrowserTab&) = delete;
	BrowserTab& operator=(const BrowserTab&) = delete;
	~BrowserTab();

	void Load(const QUrl& url) override;
	QUrl Location() const override;
	QString Title() const override;
	QIcon Icon() const override;
	void Back() override;
	void Forward() override;
	double ZoomFactor() const override ;
	void ZoomFactor(double zoom) override;
	void Reload() override;
	void InjectScript(const QString& javascript) override;
	void SingleShotTimer(int msec, std::function<void(V1::BrowserTab& tab)>&& callback) override;
	
	void SetPersona(const V1::Plugin& persona);
	
	QWebEnginePage* Page();
	
Q_SIGNALS:
	void LoadFinished(bool);
	void IconChanged(const QIcon& icon);
	void TitleChanged(const QString& title);
	
private:
	void OnLoad(bool ok);
	
private:
	std::unique_ptr<Ui::BrowserTab> m_ui;
	std::shared_ptr<V1::Plugin>     m_persona;
};

} // end of namespace
