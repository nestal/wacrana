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

Configuration::Configuration()
{
	Q_ASSERT(!m_home_page);
}

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
		
		// home page configuration
		auto home_page = LoadPlugin(doc.object()["homepage"].toObject());
		m_home_page.store(home_page.release());
	});
}

Configuration::~Configuration()
{
	if (m_loaded.valid())
		m_loaded.wait();
	
	delete m_home_page.exchange(nullptr);
}

std::unique_ptr<V1::Plugin> Configuration::LoadPlugin(const QJsonObject& config)
{
	auto factory = reinterpret_cast<V1::Factory>(QLibrary::resolve(
		config["lib"].toString(),
		config["factory"].toString().toUtf8()
	));
	
	if (!factory)
		throw std::runtime_error("cannot load library");
	
	auto plugin = (*factory)();
	plugin->OnPluginLoaded(config);
	
	return plugin;
}

V1::Plugin *Configuration::HomePage() const
{
	return m_home_page;
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

} // end of namespace
