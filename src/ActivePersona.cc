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
#include "BrightFuture/executor/QtGuiExecutor.hh"

#include <QtGui/QIcon>
#include <QDebug>

#include <mutex>

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

	void Update();
	void Detach();

private:
	BrightFuture::Executor *const m_exec;

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
	Post(tab, [ok, this](V1::BrowserTab& proxy){m_persona->OnPageLoaded(proxy, ok);});
}

void ActivePersona::OnPageIdle(V1::BrowserTab& tab)
{
	Post(tab, [this](V1::BrowserTab& proxy){m_persona->OnPageIdle(proxy);});
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
	Post(tab, [this](V1::BrowserTab& proxy){m_persona->OnAttachTab(proxy);});
}

void ActivePersona::OnDetachTab(V1::BrowserTab& tab)
{
	auto it = m_proxies.find(&tab);
	if (it != m_proxies.end())
	{
		// Remove the proxy from our list. The proxy will still be kept alive by the
		// shared_ptr inside the above lambda. It will be passed to the persona thread.
		// The proxy will be freed after the persona thread released the shared_ptr, i.e.
		// by returning from OnDetachTab().
		auto proxy = it->second;
		m_proxies.erase(it++);

		// Disable the proxy before notifying the persona thread.
		// Setting its pointer-to-real to null because the real tab is going to be
		// destroyed.
		proxy->Detach();

		m_ios.post([proxy, this]{ m_persona->OnDetachTab(*proxy); });
	}
}

std::weak_ptr<V1::BrowserTab> ActivePersona::Proxy(V1::BrowserTab& real)
{
	// In general, everything that touches the real BrowserTab should be in main thread.
	assert(std::this_thread::get_id() != m_thread.get_id());

	auto it = m_proxies.find(&real);
	if (it != m_proxies.end())
	{
		it->second->Update();
		return it->second;
	}
	else
		return {};
}

std::weak_ptr<const V1::BrowserTab> ActivePersona::Proxy(const V1::BrowserTab& real) const
{
	// In general, everything that touches the real BrowserTab should be in main thread.
	assert(std::this_thread::get_id() != m_thread.get_id());

	auto it = m_proxies.find(&real);
	return {it != m_proxies.end() ? it->second : nullptr};
}

ActivePersona::BrowserTabProxy::BrowserTabProxy(V1::BrowserTab *parent, BrightFuture::Executor *exec) :
	m_exec{exec},
	m_parent{parent}
{
	Update();
}

void ActivePersona::BrowserTabProxy::Load(const QUrl& url)
{
	BrightFuture::QtGuiExecutor::Post([self=shared_from_this(), url]
	{
		if (self->m_parent)
			self->m_parent->Load(url);
	});
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
	
	BrightFuture::QtGuiExecutor::Post([js, promise=std::move(promise), self=shared_from_this()]() mutable
	{
		if (self->m_parent)
		{
			self->Update();
			self->m_parent->InjectScript(js).then(
				[promise = std::move(promise)](BrightFuture::future<QVariant> v) mutable
				{
					promise.set_value(v.get());
				}
			);
		}
		else
			promise.set_exception(std::make_exception_ptr(std::runtime_error{"tab closed"}));
	});
	return future;
}

BrightFuture::future<QVariant> ActivePersona::BrowserTabProxy::InjectScriptFile(const QString& path)
{
	BrightFuture::promise<QVariant> promise;
	auto future = promise.get_future();
	
	BrightFuture::QtGuiExecutor::Post([path, promise=std::move(promise), self=shared_from_this()]() mutable
	{
		if (self->m_parent)
		{
			self->Update();
			self->m_parent->InjectScriptFile(path).then(
				[promise = std::move(promise)](BrightFuture::future<QVariant> v) mutable
				{
					promise.set_value(v.get());
				}
			);
		}
		else
			promise.set_exception(std::make_exception_ptr(std::runtime_error{"tab closed"}));
	});

	return future;
}

void ActivePersona::BrowserTabProxy::SingleShotTimer(TimeDuration duration, TimerCallback&& callback)
{
	BrightFuture::QtGuiExecutor::Post([self=shared_from_this(), duration, cb=std::move(callback)]() mutable
	{
		if (self->m_parent)
			self->m_parent->SingleShotTimer(duration, std::move(cb));
	});
}

void ActivePersona::BrowserTabProxy::ReportProgress(double percent)
{
	BrightFuture::QtGuiExecutor::Post([self=shared_from_this(), percent]
	{
		if (self->m_parent)
			self->m_parent->ReportProgress(percent);

		// perhaps update the fields in BrowserTabProxy?
	});
}

std::size_t ActivePersona::BrowserTabProxy::SequenceNumber() const
{
	std::unique_lock<std::mutex> lock{m_mux};
	return m_seqnum;
}

void ActivePersona::BrowserTabProxy::Update()
{
	std::unique_lock<std::mutex> lock{m_mux};
	if (m_parent)
	{
		m_location = m_parent->Location();
		m_title = m_parent->Title();
		m_seqnum = m_parent->SequenceNumber();
	}
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

void ActivePersona::BrowserTabProxy::Detach()
{
	Q_ASSERT(m_parent);
	m_parent = nullptr;
}

} // end of namespace
