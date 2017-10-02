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

#include <cassert>
#include <future>
#include <thread>
#include <type_traits>

#include <QDebug>

namespace wacrana {

template <typename T>
class ThenableFuture
{
public:
	ThenableFuture() = default;
	
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
			if (callback)
				PostMain([result=std::move(result), callback=std::move(callback)]() mutable
				{
					callback(result);
				});
		}}.detach();
	}
	
	ThenableFuture(ThenableFuture&& other) noexcept: m_promise{std::move(other.m_promise)}, m_thenned{other.m_thenned}
	{
		other.m_thenned = false;
	}
	
	~ThenableFuture()
	{
		if (!m_thenned)
			m_promise.set_value({});
	}
	
	template <typename Callable>
	void Then(Callable&& callback);
	
private:
	std::promise<std::function<void(T&)>>  m_promise;
	bool m_thenned{false};
};

template <typename Func>
auto Async(Func&& func)
{
	ThenableFuture<decltype(func())> future;
	future.Start(std::forward<Func>(func));
	return future;
}

template <>
class ThenableFuture<void>
{
public:
	ThenableFuture() = default;
	
	template <typename Func>
	void Start(Func&& func)
	{
		auto future = m_promise.get_future();
		std::thread{[func=std::forward<Func>(func), future=std::move(future)]() mutable
		{
			// Run the function "func" in the spawned thread. The return value will be
			// moved to the lambda closure to be used later.
			func();
			
			// Wait for the main thread to send us the callback function, which will be
			// called by the main thread to handle the return value of "func".
			// The callback function is specified by Then().
			auto callback = future.get();
			
			// Move the result of "func" and the callback from Then() to a lambda closure,
			// and post it back to the main thread. The main thread will execute the
			// lambda after it is received.
			if (callback)
				PostMain([callback=std::move(callback)]() mutable
				{
					callback();
				});
		}}.detach();
	}
	
	ThenableFuture(ThenableFuture&& other) noexcept: m_promise{std::move(other.m_promise)}, m_thenned{other.m_thenned}
	{
		other.m_thenned = false;
	}
	
	~ThenableFuture()
	{
		if (!m_thenned)
			m_promise.set_value({});
	}
	
	template <typename Callable>
	void Then(Callable&& callback);
	
private:
	std::promise<std::function<void()>>  m_promise;
	bool m_thenned{false};
};

template <typename T> template<typename Callable>
void ThenableFuture<T>::Then(Callable&& callback)
{
	assert(!m_thenned);
	m_promise.set_value(std::forward<Callable>(callback));
	m_thenned = true;
}

template <typename Callable>
void ThenableFuture<void>::Then(Callable&& callback)
{
	assert(!m_thenned);
	m_promise.set_value(std::forward<Callable>(callback));
	m_thenned = true;
}

} // end of namespace
