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

#include "Plugin.hpp"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QLibrary>

#include <stdexcept>
#include <iostream>

namespace wacrana {

void Configuration::Load(const QString& path)
{
	// spawn a thread to load the configuration file
	m_loaded = std::async(std::launch::async, [this, path]
	{
		// Emit Finish() at the end of this function even when exception is thrown.
		// Note that need to put a non-null pointer in unique_ptr, otherwise the
		// custom deleter will not be called.
		auto finale = [this](void*){Q_EMIT Finish();};
		std::unique_ptr<void, decltype(finale)> ptr{this, finale};
		
		QFile file(path);
		file.open(QFile::ReadOnly);
		auto doc = QJsonDocument::fromJson(file.readAll());
		
		if (doc.isNull())
			throw std::runtime_error("can't read " + path.toUtf8());
		
		// default zoom
		m_default_zoom.Set(doc.object()["default_zoom"].toDouble(1.3));
		
		// home page configuration
		auto home_page = LoadPlugin(doc.object()["homepage"].toObject());
		m_home_page.Set(home_page.release());
	});
}

Configuration::~Configuration()
{
	if (m_loaded.valid())
		m_loaded.wait();
}

std::unique_ptr<V1::Plugin> Configuration::LoadPlugin(const QJsonObject& config)
{
	using namespace std::literals;
	
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.isString() || !json_factory.isString())
		throw std::runtime_error("Invalid configuration: missing \"lib\" or \"factory\" in configuration.");
	
	QLibrary lib{json_lib.toString()};
	if (!lib.load())
		throw std::runtime_error("Cannot load library " + json_lib.toString().toStdString() + ": " + lib.errorString().toStdString());
	
	auto factory = reinterpret_cast<V1::Factory>(lib.resolve(json_factory.toString().toUtf8()));
	if (!factory)
		throw std::runtime_error("Cannot load symbol " + json_factory.toString().toStdString() + ": " + lib.errorString().toStdString());
	
	auto plugin = (*factory)();
	if (!plugin)
		throw std::runtime_error("Cannot create plugin from library " + json_lib.toString().toStdString());
	
	plugin->OnPluginLoaded(config);
	return plugin;
}

V1::Plugin *Configuration::HomePage()
{
	return m_home_page.Get();
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

double Configuration::DefaultZoom() const
{
	return m_default_zoom.Get();
}

} // end of namespace
