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
#include "Plugin.hpp"

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
	Configuration() = default;
	~Configuration() override;
	
	void Load(const QString& path);
	
	V1::Plugin* HomePage();
	double DefaultZoom() const;
	
	void GetResult();
	
Q_SIGNALS:
	/**
	 * \brief   Signal emitted after the configuration finishes loading
	 *          asynchronouosly.
	 *
	 * This signal is emitted by the thread that loads the configuration, and
	 * it's different from the thread that calls Load(). Therefore it should
	 * be connected using Qt::QueuedConnection.
	 */
	void Finish();
	
private:
	static V1::PluginPtr LoadPlugin(const QJsonObject& config);
	
private:
	std::future<void> m_loaded;
	
	OwnedFuture<V1::PluginPtr>  m_home_page;
	OwnedFuture<double>         m_default_zoom;
};

} // end of namespace

