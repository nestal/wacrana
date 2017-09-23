/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/23/17.
//

#pragma once

#include <boost/exception/exception.hpp>
#include <exception>

namespace wacrana {

struct Exception : virtual boost::exception, virtual std::exception
{
	const char* what() const noexcept override ;
};

} // end of namespace
