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
#include <thread>
#include <QDebug>

namespace wacrana {

template <typename T>
class ThenableFuture
{
public:
	ThenableFuture() = default;
	
	template <typename Func>
	explicit ThenableFuture(Func&& func)
	{
		Start(std::forward<Func>(func));
	}
	
	template <typename Func>
	void Start(Func&& func)
	{
		auto future = m_promise.get_future();
		std::thread{[func=std::forward<Func>(func), future=std::move(future)]() mutable
		{
			// Run the function "func" in the spawned thread. The return value will be
			// moved to the lambda closure to be used later.
			auto result = func();
			
			// Wait for the main thread to send us the callback function, which will be
			// called by the main thread to handle the return value of "func".
			// The callback function is specified by Then().
			auto callback = future.get();
			
			// Move the result of "func" and the callback from Then() to a lambda closure,
			// and post it back to the main thread. The main thread will execute the
			// lambda after it is received.
			PostMain([result=std::move(result), callback=std::move(callback)]() mutable
			{
				callback(result);
			});
		}}.detach();
	}
	
	template <typename Callable>
	void Then(Callable&& callback)
	{
		m_promise.set_value(std::forward<Callable>(callback));
	}
	
private:
	std::promise<std::function<void(T&)>>  m_promise;
};

} // end of namespace
