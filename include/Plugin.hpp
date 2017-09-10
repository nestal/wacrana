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

/**
 * \brief Wacrana namespace.
 *
 * Everything in wacrana will be put inside this namespace.
 */
namespace wacrana {

/**
 * \brief Wacrana version 1.x plugin interfaces.
 *
 * Interface classes defined in this namespace are supposed to be stable across
 * the 1.x releases of the browser. These interfaces will be supported until
 * v3.0 of the browser is released.
 *
 * V1.x plugins can only uses interfaces in this namespace. If a plugin uses any
 * other classes/functions in the outer wacrana namespace, it may not work with
 * a newer release, which may change the class/function in question.
 */
namespace V1 {

class MainWindow;
class BrowserTab;

/**
 * \brief Plugin interface.
 *
 * This is the interface that all plugins must implement. The browser application
 * accesses features provided by plugins by calling functions in this interface.
 *
 * A plugin is a shared library (DLL in Windows and .so in Linux) that implements
 * a factory function that returns a Plugin object.
 */
class Plugin
{
public:
	/**
	 *  \brief Name of the plugin.
	 *
	 *  This is for display only.
	 */
	virtual QString Name() const = 0;
	
	/**
	 * \brief Version of the plugin
	 *
	 * This is for display only.
	 */
	virtual QString Version() const = 0;
	
	/**
	 * \brief Callback when the plugin has been loaded.
	 * \param config JSON object read from configuration file.
	 *
	 * This is the chance for a plugin to read its configuration. The JSON object that
	 * specifies loading this plugin is passed as argument. Typically the "lib" field
	 * has the path to the DSO/DLL that implements this plugin, and the "factory"
	 * field is the symbol name of the factory function that creates this plugin.
	 * This factory function should be the only symbol that is exported/made visible
	 * by the DSO/DLL.
	 */
	virtual void OnPluginLoaded(const QJsonObject& config) = 0;
	
	/**
	 * \brief Callback when a page has finished loading.
	 * \param wnd   Browser main window, which is the parent of the tab that contains
	 *              the newly loaded web page.
	 * \param tab   The tab that contains the new page.
	 *
	 * Called when the browser has loaded a new page. The plugin can upload the page
	 * here.
	 *
	 * Note that not all plugins are notified of web page load events. For example,
	 * homepage plugins will not be notified. Plugins are required to register
	 * themselves of what sites are of interest.
	 *
	 * \todo For security concern we should allow the user to grant permission to
	 * plugins for page-load notifications.
	 */
	virtual void OnPageLoaded(MainWindow& wnd, BrowserTab& tab) = 0;
	virtual void OnAction(MainWindow&, const QString& arg) = 0;
};

typedef std::unique_ptr<Plugin> (*Factory)();

}} // end of namespace

#ifdef __GNUC__
	#define WCAPI __attribute__ ((visibility ("default")))
#elif defined _MSC_VER
	#define WCAPI __declspec(dllexport)
#endif
