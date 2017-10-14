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
#include "json.hpp"

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
	explicit PluginManager(const nlohmann::json& config = {});
	
	// copy is allowed
	PluginManager(const PluginManager&) = default;
	PluginManager(PluginManager&&) = default;
	PluginManager& operator=(const PluginManager&) = default;
	PluginManager& operator=(PluginManager&&) = default;
	
	V1::PluginPtr NewPersona(const std::string& name, V1::Context& ctx) const;
	std::vector<std::string> Find(const std::string& role) const;

private:
	std::string Load(const nlohmann::json& config);
	
	struct PluginFactory
	{
		nlohmann::json config;
		std::function<V1::PersonaFactory> factory;
		
		V1::PluginPtr operator()(V1::Context& ctx) const;
	};
	
	struct WrappedPlugin;
	
private:
	std::unordered_map<std::string, PluginFactory> m_factories;
};

} // end of namespace
