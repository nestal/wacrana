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

#include "BrowserTab.hpp"
#include "Persona.hpp"

#include "ProgressTimer.hh"
#include "TimerEventCallback.hh"

#include "ui_BrowserTab.h"
#include <memory>

class QUrl;
class QWebEnginePage;
class QStatusBar;
class QTimerEvent;

namespace wacrana {

namespace V1 {
class Persona;
}

class ActivePersona;

class BrowserTab : public QWidget, public V1::BrowserTab, private TimerEventCallback
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
	QIcon Icon() const ;
	void Back() ;
	void Forward() ;
	double ZoomFactor() const  ;
	void ZoomFactor(double zoom) ;
	void Reload() ;
	void InjectScript(const QString& javascript, ScriptCallback&& callback) override;
	void InjectScriptFile(const QString& path) override;
	void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) override;
	void ReportProgress(double percent) override;
	void LeftClick(const QPoint& pos) ;
	
	void SetPersona(V1::PersonaPtr&& persona);
	double WaitProgress() const;
	
	QWebEnginePage* Page();
	
Q_SIGNALS:
	void LoadFinished(bool);
	void IconChanged(const QIcon& icon);
	void TitleChanged(const QString& title);
	void WaitProgressUpdated(double progress, ProgressTimer::Duration remain, ProgressTimer::Duration total);
	
private:
	void OnLoadStarted();
	void OnLoadFinished(bool ok);
	void OnTimerUpdate(Clock::duration remain) override;
	void OnTimeout() override;
	void OnIdle() override;
	
	void timerEvent(QTimerEvent *event) override;
	
private:
	std::unique_ptr<Ui::BrowserTab> m_ui;
	std::unique_ptr<ActivePersona>  m_persona;
	ProgressTimer   *m_timer;
	TimerCallback   m_callback;
	QStatusBar      *m_status;
};

} // end of namespace
