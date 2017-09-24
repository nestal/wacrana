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
#include "GeneralPlugin.hpp"

#include <QHash>
#include <QtCore/QLibrary>

#include <QtCore/QFileInfo>

#include <boost/dll/import.hpp> // for import_alias

namespace wacrana {

std::size_t PluginManager::Hash::operator()(const QString& s) const
{
	return qHash(s);
}

PluginManager::PluginManager(V1::Context& ctx) :
	m_ctx{ctx}
{
}

V1::GeneralPluginPtr PluginManager::LoadPlugin(const QJsonObject& config)
{
	m_plugins.push_back(std::move(Import<V1::GeneralPluginFactory>(config).func));
	return m_plugins.back()(config, m_ctx);
}

template <typename FunctionType>
PluginManager::ImportResult<FunctionType> PluginManager::Import(const QJsonObject& config)
{
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.isString() || !json_factory.isString())
		throw std::runtime_error(R"(Invalid configuration: missing "lib" or "factory" in configuration.)");
	
	boost::filesystem::path path{json_lib.toString().toStdString()};
	
	return {
		path,
		std::function<FunctionType>{boost::dll::import_alias<FunctionType>(
			path,
			json_factory.toString().toStdString(),
			boost::dll::load_mode::append_decorations
		)}
	};
}

void PluginManager::LoadPersonaFactory(const QJsonObject& config)
{
	auto result = Import<V1::PersonaFactory>(config);
	m_persona.emplace(
		QString::fromStdString(result.path.filename().string()),
		PackedPersonaFactory{config, std::move(result.func)}
	);
}

V1::PersonaPtr PluginManager::NewPersona(const QString& name) const
{
	if (auto it = m_persona.find(name); it != m_persona.end())
		return it->second.factory(it->second.config, m_ctx);
	else
		throw std::runtime_error("not found");
}

std::vector<QString> PluginManager::Persona() const
{
	std::vector<QString> result;
	for (auto&& p : m_persona)
		result.push_back(p.first);
	return result;
}

} // end of namespace
