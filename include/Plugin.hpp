/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#pragma once

#include <memory>

class QString;
class QJsonObject;

namespace wacrana {
namespace V1 {

class MainWindow;
class BrowserTab;

/**
 * \brief Plugin interface. Implementation of plugins must implement this.
 * A plugin is a shared library (DLL in Windows and .so in Linux) that implements
 * a factory function that returns a Plugin object.
 */
class Plugin
{
public:
	/**
	 *  \brief Name of the plugin.
	 *  This is for display only.
	 */
	virtual QString Name() const = 0;
	
	/**
	 * \brief Version of the plugin
	 * This is for display only.
	 */
	virtual QString Version() const = 0;
	
	virtual void OnPluginLoaded(const QJsonObject& config) = 0;
	virtual void OnPageLoaded(MainWindow&, BrowserTab&) = 0;
	virtual void OnAction(MainWindow&, const QString& arg) = 0;
};

typedef std::unique_ptr<Plugin> (*Factory)();

}} // end of namespace
