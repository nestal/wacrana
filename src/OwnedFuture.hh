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
#include <experimental/optional>

namespace wacrana {

template <typename T>
class OwnedFuture;

template <typename T>
class OwnedFuture
{
public:
	OwnedFuture() = default;
	
	const T& Get() const
	{
		if (!m_value)
			m_value = m_future.get();
		
		return m_value.value();
	}
	
	T& Get()
	{
		if (!m_value)
			m_value = m_future.get();
		
		return m_value.value();
	}
	
	template <typename U>
	void Set(U&& value)
	{
		m_promise.set_value(std::forward<U>(value));
	}
	
private:
	mutable std::experimental::optional<T>  m_value;
	std::promise<T>                 m_promise;
	mutable std::future<T>          m_future{m_promise.get_future()};
};

template <typename T>
class OwnedFuture<T*>
{
public:
	OwnedFuture() = default;
	
	const T* Get() const
	{
		if (!m_value && m_future.valid())
			m_value.reset(m_future.get());
		
		return m_value.get();
	}
	
	T* Get()
	{
		if (!m_value && m_future.valid())
			m_value.reset(m_future.get());
		
		return m_value.get();
	}
	
	template <typename U>
	void Set(U&& value)
	{
		m_promise.set_value(std::forward<U>(value));
	}

private:
	mutable std::unique_ptr<T>      m_value;
	std::promise<T*>        m_promise;
	mutable std::future<T*> m_future{m_promise.get_future()};
};

} // end of namespace