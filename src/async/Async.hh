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
	void Then(Func&& continuation, Executor *host)
	{
		auto result = m_shared_state.share();
		auto token  = host->Add([func=std::forward<Func>(continuation), result]() mutable
		{
			func(result.get());
		});
		
		if (result.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
			host->Execute(token);
		else
			m_token.set_value(token);
	}
	
private:
	std::future<T>      m_shared_state;
	std::promise<Token> m_token;
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
			t.host->Execute(t);
		}
		
	}}.detach();
	
	return Future<T>{std::move(future), std::move(token)};
}

} // end of namespace
