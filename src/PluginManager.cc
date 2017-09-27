/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/23/17.
//

#include "PluginManager.hh"

#include <QtCore/QHash>
#include <QtCore/QJsonArray>

#include <boost/dll.hpp>
#include <boost/dll/import.hpp> // for import_alias

namespace wacrana {

std::size_t PluginManager::Hash::operator()(const QString& s) const
{
	return qHash(s);
}

PluginManager::PluginManager(const QJsonArray& config)
{
	for (auto&& p : config)
	{
		// have to load the plugin to know its name
		// anyway we have to try running the factory function once to check if there's any problem
		LoadPersonaFactory(p.toObject());
	}
}

QString PluginManager::LoadPersonaFactory(const QJsonObject& config)
{
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.isString() || !json_factory.isString())
		throw std::runtime_error(R"(Invalid configuration: missing "lib" or "factory" in configuration.)");
	
	boost::filesystem::path path{json_lib.toString().toStdString()};
	auto factory = boost::dll::import_alias<V1::PersonaFactory>(
		path,
		json_factory.toString().toStdString(),
		boost::dll::load_mode::append_decorations
	);
	
	return m_factories.emplace(
		QString::fromStdString(path.filename().string()),
		PluginFactory{config, std::move(factory)}
	).first->first;
}

V1::PluginPtr PluginManager::NewPersona(const QString& name, V1::Context& ctx) const
{
	auto it = m_factories.find(name);
	if (it != m_factories.end())
		return it->second.factory(it->second.config, ctx);
	else
		throw std::runtime_error("not found");
}

std::vector<QString> PluginManager::Find(const QString& role) const
{
	std::vector<QString> result;
	for (auto&& p : m_factories)
	{
		if (role == p.second.config["role"].toString())
			result.push_back(p.first);
	}
	return result;
}

} // end of namespace
