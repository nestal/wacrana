/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the future
    distribution for more details.
*/

//
// Created by nestal on 10/2/17.
//

#pragma once

#include "Executor.hh"

#include <future>

namespace exe {

template <typename T>
class Future
{
public:
	Future() = default;
	
	Future(std::future<T>&& shared_state, std::promise<Token>&& token) :
		m_shared_state{std::move(shared_state)},
		m_token{std::move(token)}
	{
	}
	
	template <typename Func>
	auto Then(Func&& continuation, Executor *host)
	{
		using U = decltype(continuation(T()));
		auto                next_result = std::make_shared<std::promise<U>>();
		std::promise<Token> next_token;
		
		auto next_future = next_result->get_future();
		auto result      = m_shared_state.share();
		
		auto token      = host->Add([
			result,
			func        = std::forward<Func>(continuation),
			next_result = std::move(next_result),
			next_token  = next_token.get_future().share()
		]() mutable
		{
			next_result->set_value(func(result.get()));
			
			// notify next
			if (next_token.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
			{
				auto t = next_token.get();
				if (t.host)
					t.host->Execute(t);
			}
		});
		
		m_thenned = true;
		
		if (result.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
			host->Execute(token);
		else
			m_token.set_value(token);
		
		return Future<U>{std::move(next_future), std::move(next_token)};
	}
	Future(Future&& f) :
		m_shared_state{std::move(f.m_shared_state)},
		m_token{std::move(f.m_token)},
		m_thenned{f.m_thenned}
	{
		f.m_thenned = false;
	}
	
	~Future()
	{
		if (!m_thenned)
			m_token.set_value({});
	}
	
private:
	std::future<T>      m_shared_state;
	std::promise<Token> m_token;
	bool m_thenned{false};
};

template <typename Func>
auto Async(Func&& func)
{
	using T = decltype(func());
	
	std::promise<T>     shared_state;
	std::promise<Token> token;

	auto future = shared_state.get_future();

	std::thread{[
		func    = std::forward<Func>(func),
		result  = std::move(shared_state),
		token   = token.get_future()
	] () mutable
	{
		// Run the function "func" in the spawned thread. The return value will be
		// moved to the lambda closure to be used later.
		result.set_value(func());
		
		// notify
		if (token.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
		{
			auto t = token.get();
			if (t.host)
				t.host->Execute(t);
		}
		
	}}.detach();
	
	return Future<T>{std::move(future), std::move(token)};
}

} // end of namespace
