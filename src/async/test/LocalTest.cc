/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 10/2/17.
//

#include <iostream>
#include "async/Async.hh"

#include "catch.hpp"

using namespace exe;
using namespace std::chrono_literals;

TEST_CASE( "Async simple", "[normal]" )
{
	TaskScheduler sch;
	LocalExecutor exec;
	
	auto future = Async([]
	{
		std::this_thread::sleep_for(2s);
		return 100;
	}, &exec);
	
	future.Then([](int val)
	{
		REQUIRE(val == 100);
	}, &sch, &exec);
	
	using namespace std::chrono_literals;
	while (sch.Count() > 0)
		std::this_thread::sleep_for(1s);
}