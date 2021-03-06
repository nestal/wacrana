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
#include "BrightFuture/executor/QtGuiExecutor.hh"

#include <QDebug>
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
Configuration::Configuration(const std::string& path)
{
	std::ifstream ifile{path};
	
	using json = nlohmann::json;
	auto config = json::parse(ifile);
	
	// default zoom
	m_default_zoom = config["default_zoom"];
	
	using namespace BrightFuture;
	
	// spawn a thread to load the configuration file
	m_plugin_mgr = async([this, config=std::move(config)]
	{
		return PluginManager{config["plugins"]};
	}, ::BrightFuture::TheQtGuiExecutor());
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

BrightFuture::future<PluginManager>& Configuration::Plugins()
{
	return m_plugin_mgr;
}

} // end of namespace
