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

#include "PluginManager.hh"
#include "Exception.hh"
#include "Plugin.hpp"

#include <BrightFuture/BrightFuture.hh>

#include <functional>
#include <unordered_map>

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
	explicit Configuration(const std::string& path);
	~Configuration() override = default;
	
	double DefaultZoom() const;
	
	BrightFuture::shared_future<PluginManager> Plugins() const;
	
private:
	double          m_default_zoom;
	
	BrightFuture::shared_future<PluginManager>  m_plugin_mgr;
};

} // end of namespace
