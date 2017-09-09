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

Configuration::Configuration(const QString& path)
{
	Q_ASSERT(!m_home_page);
	
	// spawn a thread to load the configuration file
	m_thread = std::thread([this, path]{
		
		try
		{
			QFile file(path);
	        file.open(QFile::ReadOnly);
	        auto doc = QJsonDocument::fromJson(file.readAll());
			
			if (doc.isNull())
				throw std::runtime_error("can't read " + path.toUtf8());
			
			// home page configuration
			auto home_page = LoadPlugin(doc.object()["homepage"].toObject());
			m_home_page.store(home_page.release());
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	});
}

Configuration::~Configuration()
{
	m_thread.join();
	
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

} // end of namespace
