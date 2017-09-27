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

#include <QObject>

#include "OwnedFuture.hh"
#include "PluginManager.hh"
#include "Exception.hh"

#include "Plugin.hpp"

#include <functional>
#include <unordered_map>

#include <boost/exception/error_info.hpp>

class QString;
class QJsonObject;

namespace wacrana {

/**
 * \brief Application configuration.
 *
 * This class loads configuration in JSON file. It will also load the plugins
 * specified in the configuration. The plugins will be loaded asynchronously
 * to reduce startup time. After it finished loading, the Finish() signal
 * will be emitted. Since the configuration is loaded asynchronously in
 * a different thread, the Finish() signal is emitted by that thread and should
 * be connect()'ed using Qt::QueuedConnection. You can call GetResult() to
 * check if the configuration is loaded successfully.
 *
 * When the browser starts, it construct a Configuration object and Load()'s it
 * before initialing the Qt web engine. This allows the Qt web engine can be
 * initialized in parallel with the configurations. If the configuration takes
 * some time to finish loading, the browser can still be started. This design
 * can ensure that the browser can be started quickly.
 */
class Configuration : public QObject
{
	Q_OBJECT

public:
	struct Error : Exception {};
	struct FileReadError : Error {};
	struct JsonParseError : Error {};

	using ErrorString   = boost::error_info<struct ErrorString_, QString>;

public:
	Configuration(const QString& path, V1::Context& ctx);
	~Configuration() override;
	
	V1::Plugin* HomePage();
	double DefaultZoom() const;
	
	void GetResult();
	
	V1::PersonaPtr MakePersona(const QString& name) const;
	std::vector<QString> Persona() const;
	
Q_SIGNALS:
	void PreFinish();
	
	/**
	 * \brief   Signal emitted after the configuration finishes loading
	 *          asynchronouosly.
	 *
	 * The constructor will load the configuration asynchronously, but this
	 * signal will not be emitted until the caller thread returns to the
	 * main loop. (The "caller" thread is the thread which calls the
	 * Configuration constructor). Therefore you must make sure this signal
	 * is connected to a slot before returning to the main loop, otherwise
	 * the signal will be missed and the slot will never be called.
	 */
	void Finish();
private:
	V1::Context&                m_ctx;

	std::future<void>           m_loaded;
	OwnedFuture<double>         m_default_zoom;
	
	OwnedFuture<PluginManager>  m_plugin_mgr;
	OwnedFuture<V1::PersonaPtr> m_home_page;
};

} // end of namespace
