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
const std::chrono::hours reseed_interval{1};
}

class ActivePersona::BrowserTabProxy : public V1::BrowserTab, public std::enable_shared_from_this<BrowserTabProxy>
{
public:
	explicit BrowserTabProxy(V1::BrowserTab *parent, BrightFuture::Executor *exec);

	void Load(const QUrl& url) override;
	QUrl Location() const override;
	QString Title() const override;

	// script injection
	BrightFuture::future<QVariant> InjectScript(const QString& js) override;
	BrightFuture::future<QVariant> InjectScriptFile(const QString& path) override;

	void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) override;
	void ReportProgress(double percent) override;
	std::size_t SequenceNumber() const override;
	std::weak_ptr<V1::BrowserTab> WeakFromThis() override;
	std::weak_ptr<const V1::BrowserTab> WeakFromThis() const override;
	BrightFuture::Executor* Executor() override;

	void Update(V1::BrowserTab *parent);

private:
	BrightFuture::Executor *m_exec;

	mutable std::mutex      m_mux;
	V1::BrowserTab  *m_parent;
	QUrl            m_location;
	QString         m_title;
	std::size_t     m_seqnum{};
};

ActivePersona::ActivePersona(V1::PluginPtr&& adaptee) :
	m_work{m_ios},
	m_timer{m_ios, reseed_interval},
	m_persona{std::move(adaptee)},
	m_thread([this]{m_ios.run();})
{
	m_timer.async_wait([this](auto ec){ReseedPersona(ec);});
}

void ActivePersona::OnPageLoaded(V1::BrowserTab& tab, bool ok)
{
	Post(tab, [ok, this](V1::BrowserTab& proxy)mutable{m_persona->OnPageLoaded(proxy, ok);});
}

void ActivePersona::OnPageIdle(V1::BrowserTab& tab)
{
	Post(tab, [this](V1::BrowserTab& proxy)mutable{m_persona->OnPageIdle(proxy);});
}

void ActivePersona::OnReseed(std::uint_fast32_t seed)
{
	m_ios.post([this, seed]{m_rand.seed(seed);});
}

std::string ActivePersona::Icon() const
{
	// constant function should be thread-safe
	return m_persona->Icon();
}

ActivePersona::~ActivePersona()
{
	m_ios.stop();
	m_thread.join();
}

void ActivePersona::ReseedPersona(boost::system::error_code)
{
	assert(std::this_thread::get_id() == m_thread.get_id());
	
	m_persona->OnReseed(m_rand());
	
	m_timer.expires_from_now(reseed_interval);
	m_timer.async_wait([this](auto ec){ReseedPersona(ec);});
}

void ActivePersona::OnAttachTab(V1::BrowserTab& tab)
{
	m_proxies.emplace(&tab, std::make_shared<BrowserTabProxy>(&tab, &m_exec));
	Post(tab, [this](V1::BrowserTab& proxy)mutable{m_persona->OnAttachTab(proxy);});
}

void ActivePersona::OnDetachTab(V1::BrowserTab& tab)
{
	auto it = m_proxies.find(&tab);
	if (it != m_proxies.end())
	{
		// Disable the proxy before notifying the persona thread
		it->second->Update(nullptr);
		m_ios.post([proxy=it->second, this](){ m_persona->OnDetachTab(*proxy); });

		// Remove the proxy from our list. The proxy will still be kept alive by the
		// shared_ptr inside the above lambda. It will be passed to the persona thread.
		// The proxy will be freed after the persona thread released the shared_ptr, i.e.
		// by returning from OnDetachTab().
		m_proxies.erase(it++);
	}
}

std::weak_ptr<V1::BrowserTab> ActivePersona::Proxy(V1::BrowserTab& real)
{
	// The BrowserTabProxy construct must be called in the GUI thread
	// because it will copy some GUI-related stuff, e.g. browser location
	// and web page title.
	assert(std::this_thread::get_id() != m_thread.get_id());

	auto it = m_proxies.find(&real);
	if (it != m_proxies.end())
	{
		it->second->Update(&real);
		return it->second;
	}
	else
		return {};
}

std::weak_ptr<const V1::BrowserTab> ActivePersona::Proxy(const V1::BrowserTab& real) const
{
	// The BrowserTabProxy construct must be called in the GUI thread
	// because it will copy some GUI-related stuff, e.g. browser location
	// and web page title.
	assert(std::this_thread::get_id() != m_thread.get_id());

	auto it = m_proxies.find(&real);
	return {it != m_proxies.end() ? it->second : nullptr};
}

ActivePersona::BrowserTabProxy::BrowserTabProxy(V1::BrowserTab *parent, BrightFuture::Executor *exec) :
	m_exec{exec},
	m_parent{parent}
{
	assert(parent);
	Update(parent);
}

void ActivePersona::BrowserTabProxy::Load(const QUrl& url)
{
	if (m_parent)
		PostMain([parent=m_parent, url]{parent->Load(url);});
}

QUrl ActivePersona::BrowserTabProxy::Location() const
{
	std::unique_lock<std::mutex> lock{m_mux};
	return m_location;
}

QString ActivePersona::BrowserTabProxy::Title() const
{
	std::unique_lock<std::mutex> lock{m_mux};
	return m_title;
}

BrightFuture::future<QVariant> ActivePersona::BrowserTabProxy::InjectScript(const QString& js)
{
	BrightFuture::promise<QVariant> promise;
	auto future = promise.get_future();
	
	if (m_parent)
		PostMain([js, promise=std::move(promise), this]() mutable
		{
			Update(m_parent);
			m_parent->InjectScript(js).then([promise=std::move(promise)](BrightFuture::future<QVariant> v) mutable
			{
				promise.set_value(v.get());
			});
		});
	else
		promise.set_exception(std::make_exception_ptr(std::runtime_error{"tab closed"}));
	return future;
}

BrightFuture::future<QVariant> ActivePersona::BrowserTabProxy::InjectScriptFile(const QString& path)
{
	BrightFuture::promise<QVariant> promise;
	auto future = promise.get_future();
	
	if (m_parent)
		PostMain([path, promise=std::move(promise), this]() mutable
		{
			Update(m_parent);
			m_parent->InjectScriptFile(path).then([promise=std::move(promise)](BrightFuture::future<QVariant> v) mutable
			{
				promise.set_value(v.get());
			});
		});
	else
		promise.set_exception(std::make_exception_ptr(std::runtime_error{"tab closed"}));
	
	return future;
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
	if (m_parent)
		PostMain([parent=m_parent, duration, cb=std::move(callback)]() mutable
		{
			parent->SingleShotTimer(duration, std::move(cb));
		});
}

void ActivePersona::BrowserTabProxy::ReportProgress(double percent)
{
	if (m_parent)
		PostMain([parent=m_parent, percent]() mutable
		{
			parent->ReportProgress(percent);
	
			// perhaps update the fields in BrowserTabProxy?
		});
}

std::size_t ActivePersona::BrowserTabProxy::SequenceNumber() const
{
	std::unique_lock<std::mutex> lock{m_mux};
	return m_seqnum;
}

void ActivePersona::BrowserTabProxy::Update(V1::BrowserTab *parent)
{
	std::unique_lock<std::mutex> lock{m_mux};
	if (parent)
	{
		assert(m_parent == parent);
		m_location = m_parent->Location();
		m_title = m_parent->Title();
		m_seqnum = m_parent->SequenceNumber();
	}
	else
		m_parent = nullptr;
}

std::weak_ptr<V1::BrowserTab> ActivePersona::BrowserTabProxy::WeakFromThis()
{
	return shared_from_this();
}

std::weak_ptr<const V1::BrowserTab> ActivePersona::BrowserTabProxy::WeakFromThis() const
{
	return shared_from_this();
}

BrightFuture::Executor *ActivePersona::BrowserTabProxy::Executor()
{
	return m_exec;
}

} // end of namespace
