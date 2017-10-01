/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 10/1/17.
//

#pragma once

#include <QtCore/QObject>

#include <future>
#include <QDebug>

namespace wacrana {

class ThenableFutureBase : public QObject
{
	Q_OBJECT

protected:
	explicit ThenableFutureBase(QObject *parent = {}) : QObject{parent}
	{
	}

Q_SIGNALS:
	void Finished();
};

template <typename T>
class ThenableFuture : private ThenableFutureBase
{
public:
	template <typename Func>
	explicit ThenableFuture(Func&& func, QObject *parent) :
		ThenableFutureBase{parent},
		m_ready_future{m_ready_promise.get_future()},
		m_future{std::async(std::launch::async, [this, func=std::forward<Func>(func)]
		{
			auto finale = [this](void*){m_ready_future.wait(); Q_EMIT Finished();};
			std::unique_ptr<void, decltype(finale)> ptr{this, finale};
			return func();
		})}
	{
	}
	
	template <typename Callable>
	void Then(Callable&& callback)
	{
		connect(this, &ThenableFuture::Finished, this, [callback=std::forward<Callable>(callback), this]
		{
			callback(m_future.get());
		}, Qt::QueuedConnection);
		
		// kick-off the deferred async task
		m_ready_promise.set_value();
	}
	
private:
	std::promise<void>  m_ready_promise;
	std::future<void>   m_ready_future;
	std::future<T>      m_future;
};

} // end of namespace
