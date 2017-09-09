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

#include <thread>
#include <atomic>

class QString;
class QJsonObject;

namespace wacrana {

namespace V1 {
class Plugin;
}

/**
 * \brief Application configuration
 * This class loads configuration in JSON file. It will also load the plugins
 * specified in the configuration. The plugins will be loaded asynchronously
 * to reduce startup time.
 */
class Configuration
{
public:
	Configuration(const QString& path);
	~Configuration();
	
	V1::Plugin* HomePage() const;
	
private:
	static std::unique_ptr<V1::Plugin> LoadPlugin(const QJsonObject& config);
	
private:
	std::thread                 m_thread;
	
	// Unfortunately C++ does not support atomic smart pointer yet
	std::atomic<V1::Plugin*> m_home_page{};
};

} // end of namespace

