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

#include "Export.hpp"
#include <cassert>

class QJsonObject;
class QString;

namespace wacrana {
namespace V1 {

class MainWindow;
class Context;

/**
 * \brief Plugin interface.
 *
 * This is the interface that all plugins must implement. The browser application
 * accesses features provided by plugins by calling functions in this interface.
 *
 * A plugin is a shared library (DLL in Windows and .so in Linux) that implements
 * a factory function that returns a Plugin object.
 */
class WCAPI GeneralPlugin
{
public:
	virtual void OnAction(MainWindow&, const QString& arg) = 0;
};

using GeneralPluginFactory = GeneralPluginPtr(const QJsonObject&, Context&);

} } // end of namespace
