/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 10/2/17.
//

#include "async/Async.hh"
#include "async/LocalExecutor.hh"

#include "catch.hpp"

using namespace exe;

TEST_CASE( "Async simple", "[normal]" )
{
	LocalExecutor exec;
	
	auto future = Async([]
	{
		return 100;
	});
	
	future.Then([](int val)
	{
		REQUIRE(val == 100);
		return std::string("abc");
	}, &exec).Then([](const std::string& s)
	{
		REQUIRE(s == "abc");
	}, &exec);
	
	using namespace std::chrono_literals;
	while (exec.Count() > 0)
		std::this_thread::sleep_for(1s);
}