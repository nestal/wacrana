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

#include <memory>

#ifdef __GNUC__
	#define WCAPI __attribute__ ((visibility ("default")))
#elif defined _MSC_VER
	#define WCAPI __declspec(dllexport)
#endif

namespace wacrana {
namespace V1 {

class Plugin;
using PersonaPtr = std::unique_ptr<V1::Plugin>;

}} // end of namespace
