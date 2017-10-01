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
	explicit ThenableFuture(Func&& func)
	{
		auto future = m_promise.get_future();
		std::thread{[func=std::forward<Func>(func), future=std::move(future)]() mutable
		{
			PostMain([result=func(), callback=future.get()]() mutable
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
