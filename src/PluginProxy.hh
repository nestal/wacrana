/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/24/17.
//

#pragma once

#include "Persona.hpp"
#include "BrowserTab.hpp"

#include <QtCore/QUrl>
#include <QtCore/QString>

#include <boost/asio.hpp>
#include <thread>

namespace wacrana {

class ActivePersona : public V1::Persona
{
public:
	explicit ActivePersona(V1::PersonaPtr&& adaptee);
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override;
	void OnPageIdle(V1::BrowserTab& tab) override;
	QIcon Icon() const override;


private:
	V1::PersonaPtr          m_adaptee;
	boost::asio::io_service m_ios;
	std::thread             m_thread;
};

class BrowserTabProxy : public V1::BrowserTab
{
public:
	explicit BrowserTabProxy(V1::BrowserTab& parent);
	BrowserTabProxy(const BrowserTabProxy&) = default;
	BrowserTabProxy(BrowserTabProxy&&) = default;
	
	void Load(const QUrl& url) override;
	QUrl Location() const override;
	QString Title() const override;
	
	// script injection
	void InjectScript(const QString& javascript, std::function<void(const QVariant&)>&& callback) override;
	void InjectScriptFile(const QString& path) override;
	
	void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) override;
	
private:
	V1::BrowserTab& m_parent;
	QUrl            m_location;
	QString         m_title;
};

} // end of namespace
