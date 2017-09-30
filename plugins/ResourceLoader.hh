/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/16/17.
//

#pragma once

#include <QtCore/QDir>

// can't call Q_INIT_RESOURCE() in extern "C" functions
// must be called in a separate function in global namespace

#define WCAPI_RESOURCE_LOADER(plugin) \
static void plugin ## InitResource()\
{\
	Q_INIT_RESOURCE(plugin);\
}\
static void plugin ## CleanupResource()\
{\
	Q_CLEANUP_RESOURCE(plugin);\
}\
\
namespace {\
struct ResourceLoader\
{\
	ResourceLoader()\
	{\
		plugin ## InitResource();\
	}\
	~ResourceLoader()\
	{\
		plugin ## CleanupResource();\
	}\
} loader;\
}
