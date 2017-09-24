/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/24/17.
//

#include "PluginProxy.hh"

#include <QtGui/QIcon>

namespace wacrana {

ActivePersona::ActivePersona(V1::PersonaPtr&& adaptee) :
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

BrowserTabProxy::BrowserTabProxy(V1::BrowserTab& parent) :
	m_parent{parent},
	m_location{m_parent.Location()},
	m_title{m_parent.Title()}
{
}

void BrowserTabProxy::Load(const QUrl&)
{
}

QUrl BrowserTabProxy::Location() const
{
	return m_location;
}

QString BrowserTabProxy::Title() const
{
	return m_title;
}

void BrowserTabProxy::InjectScript(const QString&, std::function<void(const QVariant&)>&&)
{
}

void BrowserTabProxy::InjectScriptFile(const QString&)
{
}

void BrowserTabProxy::SingleShotTimer(TimeDuration, TimerCallback&&)
{
}

} // end of namespace
