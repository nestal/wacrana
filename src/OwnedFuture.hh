/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/10/17.
//

#pragma once

#include <future>
#include <memory>

namespace wacrana {

template <typename T>
class OwnedFuture
{
public:
	OwnedFuture() = default;
	
	const T& Get() const
	{
		if (!m_has_value)
		{
			m_value = m_future.get();
			m_has_value = true;
		}
		
		return m_value;
	}
	
	T& Get()
	{
		if (!m_has_value)
		{
			m_value = m_future.get();
			m_has_value = true;
		}
		
		return m_value;
	}
	
	template <typename U>
	void Set(U&& value)
	{
		m_promise.set_value(std::forward<U>(value));
	}
	
private:
	mutable bool m_has_value{false};
	mutable T    m_value{};
	std::promise<T>                 m_promise;
	mutable std::future<T>          m_future{m_promise.get_future()};
};

} // end of namespace
