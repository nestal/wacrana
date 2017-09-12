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

#include <boost/optional.hpp>

#include <future>
#include <memory>

namespace wacrana {

/**
 * \brief	Wrapper for a future and promise its value
 * @tparam T	The type of the value being managed
 *
 * This class is only safe for a single "consumer" thread. Need to
 * change to shared_future for multiple consumer threads.
 */
template <typename T>
class OwnedFuture
{
public:
	OwnedFuture() = default;
	
	const T& Get() const
	{
		if (!m_value)
			m_value.emplace(m_future.get());

		return m_value.value();
	}
	
	T& Get()
	{
		if (!m_value)
			m_value.emplace(m_future.get());

		return m_value.value();
	}
	
	template <typename U>
	void Set(U&& value)
	{
		m_promise.set_value(std::forward<U>(value));
	}
	
private:
	mutable boost::optional<T>      m_value{};
	std::promise<T>                 m_promise;
	mutable std::future<T>          m_future{m_promise.get_future()};
};

} // end of namespace
