/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the future
    distribution for more details.
*/

//
// Created by nestal on 10/2/17.
//

#include "LocalExecutor.hh"

#include <cassert>

namespace exe {


Token LocalExecutor::Add(Callback&& func)
{
	auto seq = m_seq++;
	m_callbacks.emplace(seq, std::move(func));
	return {this, seq};
}

void LocalExecutor::Execute(Token token)
{
	assert(token.host == this);
	auto it = m_callbacks.find(token.event);
	if (it != m_callbacks.end())
	{
		it->second();
		it = m_callbacks.erase(it);
	}
}

std::size_t LocalExecutor::Count() const
{
	return m_callbacks.size();
}

} // end of namespace
