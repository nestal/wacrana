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

#include "FunctorEvent.hh"

#include <future>
#include <QDebug>

namespace wacrana {

template <typename T>
class ThenableFuture
{
public:
	template <typename Func>
	explicit ThenableFuture(Func&& func) :
		m_ready_future{m_ready_promise.get_future()},
		m_future{std::async(std::launch::async, [func=std::forward<Func>(func), this]
		{
			auto result = func();
			PostMain([r=std::move(result), this]() mutable
			{
				m_ready_future.get()(r);
			});
			return result;
		})}
	{
	}
	
	template <typename Callable>
	void Then(Callable&& callback)
	{
		m_ready_promise.set_value(std::forward<Callable>(callback));
	}
	
private:
	std::promise<std::function<void(T&)>>  m_ready_promise;
	std::future<std::function<void(T&)>>   m_ready_future;
	std::future<T>      m_future;
};

} // end of namespace
