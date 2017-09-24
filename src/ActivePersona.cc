/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/24/17.
//

#include "ActivePersona.hh"
#include "FunctorEvent.hh"

#include <QCoreApplication>
#include <QtGui/QIcon>
#include <QDebug>
#include <iostream>

namespace wacrana {

class ActivePersona::BrowserTabProxy : public V1::BrowserTab
{
public:
	explicit BrowserTabProxy(V1::BrowserTab& parent);
	BrowserTabProxy(const BrowserTabProxy&) = default;
	BrowserTabProxy(BrowserTabProxy&&) = default;
	
	void Load(const QUrl& url) override;
	QUrl Location() const override;
	QString Title() const override;
	
	// script injection
	void InjectScript(const QString& js, ScriptCallback&& callback) override;
	void InjectScriptFile(const QString& path) override;
	
	void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) override;
	
private:
	V1::BrowserTab& m_parent;
	QUrl            m_location;
	QString         m_title;
};

ActivePersona::ActivePersona(V1::PersonaPtr&& adaptee) :
	m_work{m_ios},
	m_adaptee{std::move(adaptee)},
	m_thread([this]{m_ios.run();})
{
}

void ActivePersona::OnPageLoaded(V1::BrowserTab& tab, bool ok)
{
	BrowserTabProxy proxy{tab};
	m_ios.post([tab=std::move(proxy), ok, this]()mutable{m_adaptee->OnPageLoaded(tab, ok);});
}

void ActivePersona::OnPageIdle(V1::BrowserTab& tab)
{
	BrowserTabProxy proxy{tab};
	m_ios.post([tab=std::move(proxy), this]()mutable{m_adaptee->OnPageIdle(tab);});
}

QIcon ActivePersona::Icon() const
{
	// constant function should be thread-safe
	return m_adaptee->Icon();
}

ActivePersona::~ActivePersona()
{
	m_ios.stop();
	m_thread.join();
}

void ActivePersona::Post(V1::BrowserTab& tab, std::function<void(V1::BrowserTab&)>&& callback)
{
	BrowserTabProxy proxy{tab};
	m_ios.post([px=std::move(proxy), cb=std::move(callback)]() mutable
	{
		if (cb)
			cb(px);
	});
}

ActivePersona::BrowserTabProxy::BrowserTabProxy(V1::BrowserTab& parent) :
	m_parent{parent},
	m_location{m_parent.Location()},
	m_title{m_parent.Title()}
{
}

void ActivePersona::BrowserTabProxy::Load(const QUrl& url)
{
	PostMain([&parent=this->m_parent, url]{parent.Load(url);});
}

QUrl ActivePersona::BrowserTabProxy::Location() const
{
	return m_location;
}

QString ActivePersona::BrowserTabProxy::Title() const
{
	return m_title;
}

void ActivePersona::BrowserTabProxy::InjectScript(const QString& js, ScriptCallback&& callback)
{
	// BrowserTabProxy is a temporary object. "this" will be destroyed when the callback
	// is invoked. Therefore we capture &parent instead of capturing "this".
	PostMain([&parent=this->m_parent, js, cb=std::move(callback)]()mutable{parent.InjectScript(js, std::move(cb));});
}

void ActivePersona::BrowserTabProxy::InjectScriptFile(const QString& path)
{
	PostMain([&parent=this->m_parent, path]{parent.InjectScriptFile(path);});
}

void ActivePersona::BrowserTabProxy::SingleShotTimer(TimeDuration duration, TimerCallback&& callback)
{
	PostMain([&parent=this->m_parent, duration, cb=std::move(callback)]() mutable
	{
		parent.SingleShotTimer(duration, std::move(cb));
	});
}

} // end of namespace
