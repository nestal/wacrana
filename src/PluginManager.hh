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
	
	void LoadPlugin(const QJsonObject& config);
	V1::PersonaPtr NewPersona(const QString& name) const;
	
	std::vector<QString> Persona() const;
	
private:
	struct Hash
	{
		std::size_t operator()(const QString& s) const;
	};
	
	struct PackedFactory
	{
		QJsonObject config;
		V1::Factory factory;
	};
	
	V1::Context&    m_ctx;
	
	std::unordered_map<
		QString,
		PackedFactory,
		Hash
	> m_plugins;
};

} // end of namespace
