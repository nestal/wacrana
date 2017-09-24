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
#include <boost/dll.hpp>

#include <QtCore/QString>
#include <QtCore/QJsonObject>

#include <vector>
#include <unordered_map>
#include <memory>

namespace wacrana {

namespace V1 {
class Context;
}

class PluginManager
{
public:
	explicit PluginManager(V1::Context& ctx);
	
	V1::GeneralPluginPtr LoadPlugin(const QJsonObject& config);
	
	void LoadPersonaFactory(const QJsonObject& config);
	V1::PersonaPtr NewPersona(const QString& name) const;
	std::vector<QString> Persona() const;
	
private:
	struct Lib
	{
		QFunctionPointer func;
		QString          filename;
	};
	static Lib Resolve(const QJsonObject& config);
	
private:
	struct Hash
	{
		std::size_t operator()(const QString& s) const;
	};
	
	struct PackedPersonaFactory
	{
		QJsonObject config;
		std::function<V1::PersonaFactoryType> factory;
	};
	
	V1::Context&    m_ctx;
	
	std::unordered_map<
		QString,
		PackedPersonaFactory,
		Hash
	> m_persona;
};

} // end of namespace
