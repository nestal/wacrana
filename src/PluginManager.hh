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

#include "Persona.hpp"
#include "GeneralPlugin.hpp"

#include <QtCore/QString>
#include <QtCore/QJsonObject>

#include <boost/filesystem/path.hpp>

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace wacrana {

namespace V1 {
class Context;
}

class PluginManager
{
public:
	PluginManager() = default;
	
	V1::GeneralPluginPtr LoadPlugin(const QJsonObject& config, V1::Context& ctx);
	
	void LoadPersonaFactory(const QJsonObject& config);
	V1::PersonaPtr NewPersona(const QString& name, V1::Context& ctx) const;
	std::vector<QString> Persona() const;

private:
	struct Hash
	{
		std::size_t operator()(const QString& s) const;
	};
	
	struct PackedPersonaFactory
	{
		QJsonObject config;
		std::function<V1::PersonaFactory> factory;
	};
	
	template <typename FunctionType>
	struct ImportResult
	{
		boost::filesystem::path     path;
		std::function<FunctionType> func;
	};
	
	template <typename FunctionType>
	static ImportResult<FunctionType> Import(const QJsonObject& config);

private:
	std::vector<std::function<V1::GeneralPluginFactory>> m_plugins;
	std::unordered_map<
		QString,
		PackedPersonaFactory,
		Hash
	> m_persona;
};

} // end of namespace
