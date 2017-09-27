/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/17/17.
//

#pragma once

#include <random>

namespace wacrana {
namespace V1 {

class Context
{
public:
	// random generator
	virtual std::uint_fast32_t Random() = 0;
};

}} // end of namespace
