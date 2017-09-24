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
	return V1::LoadPlugin(reinterpret_cast<V1::PluginFactory>(Resolve(config).func), config, m_ctx);
}

void PluginManager::LoadPersonaFactory(const QJsonObject& config)
{
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.isString() || !json_factory.isString())
		throw std::runtime_error(R"(Invalid configuration: missing "lib" or "factory" in configuration.)");
	
	auto lib = Resolve(config);
	m_persona.emplace(
		QFileInfo{lib.filename}.baseName(),
		PackedPersonaFactory{config, boost::dll::import_alias<V1::PersonaFactoryType>(
			json_lib.toString().toStdString(),
			json_factory.toString().toStdString(),
			boost::dll::load_mode::append_decorations
		)}
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

PluginManager::Lib PluginManager::Resolve(const QJsonObject& config)
{
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.isString() || !json_factory.isString())
		throw std::runtime_error(R"(Invalid configuration: missing "lib" or "factory" in configuration.)");
	
	// Qt doesn't use exceptions... how lame
	QLibrary lib{json_lib.toString()};
	if (!lib.load())
		throw std::runtime_error("Cannot load library " + json_lib.toString().toStdString() + ": " + lib.errorString().toStdString());
	
	auto symbol = lib.resolve(json_factory.toString().toUtf8());
	if (!symbol)
		throw std::runtime_error("Cannot load symbol " + json_factory.toString().toStdString() + ": " + lib.errorString().toStdString());
	
	return {symbol, lib.fileName()};
}

} // end of namespace
