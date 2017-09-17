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
#include <cassert>

class QString;
class QJsonObject;
class QIcon;

#ifdef __GNUC__
	#define WCAPI __attribute__ ((visibility ("default")))
#elif defined _MSC_VER
	#define WCAPI __declspec(dllexport)
#endif

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
class Context;

class Plugin;
using PluginPtr = std::unique_ptr<V1::Plugin>;

/**
 * \brief Plugin interface.
 *
 * This is the interface that all plugins must implement. The browser application
 * accesses features provided by plugins by calling functions in this interface.
 *
 * A plugin is a shared library (DLL in Windows and .so in Linux) that implements
 * a factory function that returns a Plugin object.
 */
class WCAPI Plugin
{
public:
	virtual ~Plugin() = default;
	
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
	virtual void OnPageLoaded(BrowserTab& tab, bool ok) = 0;
	virtual void OnAction(MainWindow&, const QString& arg) = 0;
	virtual QIcon Icon() const = 0;
	
	virtual PluginPtr New() const = 0;
};

/**
 * In windows, DLL export functions cannot return C++ classes. Since the factory
 * function must be exported by the DLL, it cannot return a smart pointer.
 */
typedef Plugin* (*Factory)(const QJsonObject&, Context&);

inline PluginPtr LoadPlugin(Factory func, const QJsonObject& config, Context& ctx)
{
	assert(func);
	return PluginPtr{(*func)(config, ctx)};
}

}} // end of namespace

