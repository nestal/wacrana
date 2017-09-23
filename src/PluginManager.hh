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

#include <unordered_map>
#include <memory>

namespace wacrana {

class Context;

class PluginManager
{
public:
	PluginManager(Context& ctx);
	
	void LoadPlugin(const QJsonObject& config);
	
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
	
	Context&    m_ctx;
	
	std::unordered_map<
		QString,
		V1::PluginPtr,
		Hash
	> m_plugins;
};

} // end of namespace
