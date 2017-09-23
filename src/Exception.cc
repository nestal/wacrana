/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/23/17.
//

#include "Exception.hh"

#include <boost/exception/diagnostic_information.hpp>

namespace wacrana {

const char *Exception::what() const noexcept
{
	return boost::diagnostic_information_what(*this);
}

} // end of namespace
