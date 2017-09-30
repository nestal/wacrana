/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#include "Configuration.hh"

#include "json.hpp"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>

#include <boost/throw_exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_api_function.hpp>

#include <memory>
#include <stdexcept>
#include <fstream>

namespace wacrana {

/**
 * \brief Asynchronously read configurations from JSON file and load all plugins
 * \param path Path to the JSON configuration file
 *
 * The Finish() signal will be emitted when finish loading the configuration.
 * To check for any errors from the configuration file, call GetResult() in
 * the slot that connects to Finish(). Any exception thrown when loading
 * the configurations will be thrown by GetResult().
 *
 * It is OK to call member functions (e.g. DefaultZoom()) in this class to
 * get the configuration values even before the configuration file is finished
 * loading. These member functions will just block until the corresponding
 * configuration value is loaded. They will not wait until the whole file
 * has been finished loaded (i.e. after Finish() is emitted). Instead they
 * just block until the configuration value of interest has been loaded.
 * Therefore, this function should load the configuration values in the
 * same order as they are needed to ensure maximum performance.
 */
Configuration::Configuration(const std::string& path, V1::Context& ctx) : m_ctx{ctx}
{
	std::ifstream ifile{path};
	
	using json = nlohmann::json;
	auto config = json::parse(ifile);
	
	// default zoom
	m_default_zoom = config["default_zoom"];
	
	// Make sure the PreFinish() signal is emitted before the async function starts.
	// Connect using QueuedConnection to ensure the Finish() signal will be emitted
	// after returning to the main loop. This is to ensure the signal will not be
	// missed even if we connect it _after_ it is emitted. Just make sure to connect
	// it before returning to the main loop.
	connect(this, &Configuration::PreFinish, this, &Configuration::Finish, Qt::QueuedConnection);
	
	// spawn a thread to load the configuration file
	m_plugin_mgr = std::async(std::launch::async, [this, config=std::move(config)]
	{
		// Emit PreFinish() at the end of this function even when exception is thrown.
		// Note that need to put a non-null pointer in unique_ptr, otherwise the
		// custom deleter will not be called.
		auto finale = [this](void*){Q_EMIT PreFinish();};
		std::unique_ptr<void, decltype(finale)> ptr{this, finale};
		
		return PluginManager{config["plugins"]};
	}).share();
}

/**
 * \brief Get the default zoom factor.
 * \return Default zoom factor.
 *
 * This functions returns the default zoom factor, which will be applied to all tabs
 * after they are loaded. Zoom factor of 1.0 means no zoom.
 *
 * Like other getters in this class, this function will block until the zoom factory value
 * has been loaded asynchronously from JSON configuration file.
 */
double Configuration::DefaultZoom() const
{
	return m_default_zoom;
}

V1::PluginPtr Configuration::MakePersona(const QString& name) const
{
	return m_plugin_mgr.get().NewPersona(name.toStdString(), m_ctx);
}

std::vector<QString> Configuration::Find(const QString& role) const
{
	std::vector<QString> result;
	for (auto&& s : m_plugin_mgr.get().Find(role.toStdString()))
		result.push_back(QString::fromStdString(s));
	return result;
}

} // end of namespace
