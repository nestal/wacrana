/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/27/17.
//

#include "Beethoven.hh"
#include "plugins/ResourceLoader.hh"

#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS

namespace wacrana {

BOOST_DLL_ALIAS(
    wacrana::Beethoven::Create, // <-- this function is exported with...
    Load                        // <-- ...this alias name
)

} // end of namespace

WCAPI_RESOURCE_LOADER(Beethoven)
