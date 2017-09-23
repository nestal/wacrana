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
#include "Context.hh"

#include <QHash>
#include <QtCore/QLibrary>

#include <QtCore/QFileInfo>

namespace wacrana {

PluginManager::PluginManager(Context& ctx) :
	m_ctx{ctx}
{
}

std::size_t PluginManager::Hash::operator()(const QString& s) const
{
	return qHash(s);
}

void PluginManager::LoadPlugin(const QJsonObject& config)
{
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.isString() || !json_factory.isString())
		throw std::runtime_error(R"(Invalid configuration: missing "lib" or "factory" in configuration.)");
	
	// Qt doesn't use exceptions... how lame
	QLibrary lib{json_lib.toString()};
	if (!lib.load())
		throw std::runtime_error("Cannot load library " + json_lib.toString().toStdString() + ": " + lib.errorString().toStdString());
	
	auto factory = reinterpret_cast<V1::Factory>(lib.resolve(json_factory.toString().toUtf8()));
	if (!factory)
		throw std::runtime_error("Cannot load symbol " + json_factory.toString().toStdString() + ": " + lib.errorString().toStdString());
	
	PackedFactory packed{config, factory};
	m_plugins.emplace(QFileInfo{lib.fileName()}.baseName(), packed);
}

V1::PersonaPtr PluginManager::NewPersona(const QString& name) const
{
	if (auto it = m_plugins.find(name); it != m_plugins.end())
		return V1::LoadPlugin(it->second.factory, it->second.config, m_ctx);
	else
		throw std::runtime_error("not found");
}

} // end of namespace
