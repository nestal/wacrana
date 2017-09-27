/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/23/17.
//

#pragma once

#include "Plugin.hpp"

#include <QtCore/QString>
#include <QtCore/QJsonObject>

#include <boost/filesystem/path.hpp>

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

class QJsonArray;

namespace wacrana {

namespace V1 {
class Context;
}

class PluginManager
{
public:
	explicit PluginManager(const QJsonArray& config);
	
	// copy is allowed
	PluginManager(const PluginManager&) = default;
	PluginManager(PluginManager&&) = default;
	PluginManager& operator=(const PluginManager&) = default;
	PluginManager& operator=(PluginManager&&) = default;
	
	V1::PluginPtr NewPersona(const QString& name, V1::Context& ctx) const;
	std::vector<QString> Find(const QString& role) const;

private:
	QString LoadPersonaFactory(const QJsonObject& config);
	
	struct Hash
	{
		std::size_t operator()(const QString& s) const;
	};
	
	struct PluginFactory
	{
		QJsonObject config;
		std::function<V1::PersonaFactory> factory;
	};
	
private:
	std::unordered_map<
		QString,
		PluginFactory,
		Hash
	> m_factories;
};

} // end of namespace
