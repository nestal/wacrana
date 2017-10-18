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

#include "Plugin.hpp"
#include "BrowserTab.hpp"
#include "FunctorEvent.hh"
#include "AsioExecutor.hh"

#include <QtCore/QUrl>
#include <QtCore/QString>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#include <mutex>
#include <cassert>
#include <random>
#include <thread>
#include <unordered_map>

namespace wacrana {

/**
 * \brief Wrapper around a persona that uses a separate threads to run it.
 *
 * This class is the heart of the multi-thread architecture of the wacrana
 * personas. Is is the "active" version of a persona
 */
class ActivePersona : public V1::Plugin
{
public:
	explicit ActivePersona(V1::PluginPtr&& adaptee);
	~ActivePersona() override;
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override;
	void OnPageIdle(V1::BrowserTab& tab) override;
	std::string Icon() const override;
	void OnReseed(std::uint_fast32_t seed) override;

	void OnAttachTab(V1::BrowserTab& tab) override;
	void OnDetachTab(V1::BrowserTab& tab) override;

	template <typename Func>
	void Post(V1::BrowserTab& real, Func&& callback)
	{
		// The BrowserTabProxy construct must be called in the GUI thread
		// because it will copy some GUI-related stuff, e.g. browser location
		// and web page title.
		assert(std::this_thread::get_id() != m_thread.get_id());
		auto it = m_proxies.find(&real);
		assert(it != m_proxies.end());

		it->second->Update(&real);
		
		// Move the callback function and the BrowserTabProxy to the lambda
		// function, and call use callback function in the persona thread.
		m_ios.post([proxy=it->second, cb=std::forward<Func>(callback)]() mutable
		{
			cb(*proxy);
		});
	}

	template <typename Func>
	void Post(Func&& callback)
	{
		// The BrowserTabProxy construct must be called in the GUI thread
		// because it will copy some GUI-related stuff, e.g. browser location
		// and web page title.
		assert(std::this_thread::get_id() != m_thread.get_id());
		m_ios.post(std::forward<Func>(callback));
	}

private:
	class BrowserTabProxy : public V1::BrowserTab, public std::enable_shared_from_this<BrowserTabProxy>
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
	
	void ReseedPersona(boost::system::error_code ec);
	
private:
	V1::PluginPtr                   m_persona;
	boost::asio::io_service         m_ios;
	boost::asio::io_service::work   m_work;
	boost::asio::steady_timer       m_timer;
	AsioExecutor                    m_exec{m_ios};

	std::unordered_map<V1::BrowserTab*, std::shared_ptr<BrowserTabProxy>> m_proxies;

	// this must be the last
	std::thread                     m_thread;
	std::mt19937                    m_rand;
};

} // end of namespace
