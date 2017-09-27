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
Configuration::Configuration(const QString& path, V1::Context& ctx) : m_ctx{ctx}
{
	// Make sure the PreFinish() signal is emitted before the async function starts.
	// Connect using QueuedConnection to ensure the Finish() signal will be emitted
	// after returning to the main loop. This is to ensure the signal will not be
	// missed even if we connect it _after_ it is emitted. Just make sure to connect
	// it before returning to the main loop.
	connect(this, &Configuration::PreFinish, this, &Configuration::Finish, Qt::QueuedConnection);
	
	// spawn a thread to load the configuration file
	m_loaded = std::async(std::launch::async, [this, path]
	{
		// Emit PreFinish() at the end of this function even when exception is thrown.
		// Note that need to put a non-null pointer in unique_ptr, otherwise the
		// custom deleter will not be called.
		auto finale = [this](void*){Q_EMIT PreFinish();};
		std::unique_ptr<void, decltype(finale)> ptr{this, finale};
		
		try
		{
			QFile file(path);
			if (!file.open(QFile::ReadOnly))
				BOOST_THROW_EXCEPTION(FileReadError()
					<< boost::errinfo_file_name{path.toStdString()}
					<< boost::errinfo_api_function{"QFile::open"}
					<< ErrorString{file.errorString()}
				);
				
			QJsonParseError err;
			auto doc = QJsonDocument::fromJson(file.readAll(), &err);
			
			if (doc.isNull())
				BOOST_THROW_EXCEPTION(JsonParseError()
					<< boost::errinfo_file_name{path.toStdString()}
					<< boost::errinfo_api_function{"QJsonDocument::fromJson"}
					<< ErrorString{err.errorString()}
				);
			
			// default zoom
			m_default_zoom.Set(doc.object()["default_zoom"].toDouble(1.3));
			
			// plugins
			m_plugin_mgr.Set(PluginManager{doc.object()["plugins"].toArray()});
		}
		catch (...)
		{
			// this is not good, must remember to set exception to all config items
			m_default_zoom.OnException(std::current_exception());
			m_plugin_mgr.OnException(std::current_exception());
			
			throw;
		}
	});
}

Configuration::~Configuration()
{
	if (m_loaded.valid())
		m_loaded.wait();
}

/**
 * \brief Check if there is any exception thrown when loading the configuration.
 * If an exception was thrown when the configuration was loaded asynchronously in a different
 * thread, those exception will be thrown by this function.
 *
 * If the configuration has not been finished loading, this function will block until it
 * does. Typically this function is called in the slot that connects to Finish() or any
 * time afterwards.
 */
void Configuration::GetResult()
{
	if (m_loaded.valid())
		m_loaded.get();
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
	return m_default_zoom.Get();
}

V1::PluginPtr Configuration::MakePersona(const QString& name) const
{
	return m_plugin_mgr.Get().NewPersona(name, m_ctx);
}

std::vector<QString> Configuration::Find(const QString& role) const
{
	return m_plugin_mgr.Get().Find(role);
}

} // end of namespace
