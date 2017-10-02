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

#include <functional>

namespace exe {

class Executor;

struct Token
{
	Executor        *host;
	std::intptr_t   event;
};

class Executor
{
public:
	using Callback  = std::function<void()>;
	
public:
	virtual Token Add(Callback&& callback) = 0;
	virtual void Execute(Token token) = 0;
};

} // end of namespace
