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
	
	template <typename Func>
	void Start(Func&& func)
	{
		std::thread{[func=std::forward<Func>(func), this]() mutable
		{
			// Run the function "func" in the spawned thread. The return value will be
			// moved to the lambda closure to be used later.
			m_shared_state.set_value(func());
			
			// notify
			auto token = m_token.get_future();
			if (token.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
			{
				auto t = token.get();
				t.host->Execute(t);
			}
			
		}}.detach();
	}
	
	template <typename Func>
	void Then(Func&& continuation, Executor *host)
	{
		auto shared_state   = m_shared_state.get_future().share();
		auto token          = host->Add([func=std::forward<Func>(continuation), shared_state]() mutable
		{
			func(shared_state.get());
		});
		
		if (shared_state.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
			host->Execute(token);
		else
			m_token.set_value(token);
	}
	
private:
	std::promise<T>     m_shared_state;
	std::promise<Token> m_token;
};

} // end of namespace
