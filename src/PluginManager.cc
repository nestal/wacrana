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

#include <boost/dll.hpp>
#include <boost/dll/import.hpp> // for import_alias

namespace wacrana {

PluginManager::PluginManager(const nlohmann::json& config)
{
	for (auto&& p : config)
	{
		// have to load the plugin to know its name
		// anyway we have to try running the factory function once to check if there's any problem
		Load(p);
	}
}

std::string PluginManager::Load(const nlohmann::json& config)
{
	auto&& json_lib      = config["lib"];
	auto&& json_factory  = config["factory"];
	
	if (!json_lib.is_string() || !json_factory.is_string())
		throw std::runtime_error(R"(Invalid configuration: missing "lib" or "factory" in configuration.)");
	
	boost::filesystem::path path{json_lib.get<std::string>()};
	auto factory = boost::dll::import_alias<V1::PersonaFactory>(
		path,
		json_factory,
		boost::dll::load_mode::append_decorations
	);
	
	return m_factories.emplace(
		path.filename().string(),
		PluginFactory{config, std::move(factory)}
	).first->first;
}

/*!
 * \brief A plugin that owned a copy of the factory function that creates it.
 *
 * The factory function is loaded from the dynamic library. If all copies of the function function
 * is destroyed, the dynamic library will be unloaded. If we put the factory and the plugin together
 * we can make sure we don't accidentally unloaded the dynamic library when the plugin is still
 * in use.
 */
struct PluginManager::WrappedPlugin : public V1::Plugin
{
public:
	WrappedPlugin(const std::function<V1::PersonaFactory>& factory, const nlohmann::json& config, V1::Context& ctx) :
		m_factory{factory},
		m_plugin{m_factory(config, ctx)}
	{
	}
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override
	{
		return m_plugin->OnPageLoaded(tab, ok);
	}
	void OnPageIdle(V1::BrowserTab& tab) override
	{
		return m_plugin->OnPageIdle(tab);
	}
	std::string Icon() const override
	{
		return m_plugin->Icon();
	}
	void OnReseed(std::uint_fast32_t seed) override
	{
		m_plugin->OnReseed(seed);
	}
	
private:
	std::function<V1::PersonaFactory> m_factory;
	V1::PluginPtr m_plugin;
};

V1::PluginPtr PluginManager::NewPersona(const std::string& name, V1::Context& ctx) const
{
	auto it = m_factories.find(name);
	if (it != m_factories.end())
		return std::make_unique<WrappedPlugin>(it->second.factory, it->second.config, ctx);
	else
		throw std::runtime_error("not found");
}

std::vector<std::string> PluginManager::Find(const std::string& role) const
{
	std::vector<std::string> result;
	for (auto&& p : m_factories)
	{
		if (role == p.second.config["role"].get<std::string>())
			result.push_back(p.first);
	}
	return result;
}

V1::PluginPtr PluginManager::PluginFactory::operator()(V1::Context& ctx) const
{
	return std::make_unique<WrappedPlugin>(factory, config, ctx);
}
} // end of namespace
