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

#include <QtGui/QIcon>
#include <QDebug>
#include <iostream>

namespace wacrana {

namespace {
const std::chrono::milliseconds timer_interval{500};
}

ActivePersona::ActivePersona(V1::PersonaPtr&& adaptee) :
	m_work{m_ios},
	m_timer{m_ios, timer_interval},
	m_persona{std::move(adaptee)},
	m_thread([this]{m_ios.run();})
{
	m_timer.async_wait([this](auto ec){OnTimer(ec);});
}

void ActivePersona::OnPageLoaded(V1::BrowserTab& tab, bool ok)
{
	Post(tab, [ok, this](V1::BrowserTab& proxy)mutable{m_persona->OnPageLoaded(proxy, ok);});
}

void ActivePersona::OnPageIdle(V1::BrowserTab& tab)
{
	Post(tab, [this](V1::BrowserTab& proxy)mutable{m_persona->OnPageIdle(proxy);});
}

QIcon ActivePersona::Icon() const
{
	// constant function should be thread-safe
	return m_persona->Icon();
}

ActivePersona::~ActivePersona()
{
	m_ios.stop();
	m_thread.join();
}

void ActivePersona::OnTimer(boost::system::error_code)
{
	assert(std::this_thread::get_id() == m_thread.get_id());
	
	m_timer.expires_from_now(timer_interval);
	m_timer.async_wait([this](auto ec){OnTimer(ec);});
}

ActivePersona::BrowserTabProxy::BrowserTabProxy(V1::BrowserTab& parent) :
	m_parent{parent},
	m_location{m_parent.Location()},
	m_title{m_parent.Title()}
{
}

void ActivePersona::BrowserTabProxy::Load(const QUrl& url)
{
	PostMain([&parent=m_parent, url]{parent.Load(url);});
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
	PostMain([&parent=m_parent, js, cb=std::move(callback)]() mutable
	{
		parent.InjectScript(js, std::move(cb));
	});
}

void ActivePersona::BrowserTabProxy::InjectScriptFile(const QString& path)
{
	PostMain([&parent=m_parent, path]{parent.InjectScriptFile(path);});
}

void ActivePersona::BrowserTabProxy::SingleShotTimer(TimeDuration duration, TimerCallback&& callback)
{
	// call ActivePersona::SingleShotTimer() here
	// find a way to pass the m_parent reference to ActivePersona
	// when the timer expires, use this m_parent reference to create BrowserTabProxy again
	// (oops but we can only create BroswerTabProxy in main threads)
	// we don't want to use the old BrowserTabProxy (i.e. *this) again when the timer fires
	// because most of the stuff stored inside *this will be invalid (e.g. title, location).
	// this is not a good idea afterall. giving up.

	PostMain([&parent=m_parent, duration, cb=std::move(callback)]() mutable
	{
		parent.SingleShotTimer(duration, std::move(cb));
	});
}

void ActivePersona::BrowserTabProxy::ReportProgress(double percent)
{
	PostMain([&parent=m_parent, percent]() mutable
	{
		parent.ReportProgress(percent);

		// perhaps update the fields in BrowserTabProxy?
	});
}

} // end of namespace
