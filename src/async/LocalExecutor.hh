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
#include <unordered_map>

namespace exe {

class LocalExecutor : public Executor
{
public:
	LocalExecutor() = default;
	
	Token Add(Callback&& func) override;
	void Execute(Token token) override;
	
	std::size_t Count() const;
	
private:
	std::intptr_t                               m_seq{0};
	std::unordered_map<std::intptr_t, Callback> m_callbacks;
};

} // end of namespace
