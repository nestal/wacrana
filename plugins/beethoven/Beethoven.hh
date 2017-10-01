/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/12/17.
//

#pragma once

#include "Plugin.hpp"

#include "Wait.hh"

#include <memory>
#include <random>

class QString;

namespace wacrana {

class Beethoven : public V1::Plugin
{
public:
	Beethoven(const nlohmann::json& config, std::mt19937::result_type seed);
	~Beethoven() override;
	
	// copy and moves are not allowed. no one uses these anyway.
	Beethoven(const Beethoven&) = delete;
	Beethoven(Beethoven&&) = delete;
	Beethoven& operator=(const Beethoven&) = delete;
	Beethoven& operator=(Beethoven&&) = delete;
	
	void OnPageLoaded(V1::BrowserTab&, bool) override ;
	void OnPageIdle(V1::BrowserTab& tab) override;
	std::string Icon() const override;
	void OnReseed(std::uint_fast32_t seed) override;

	static V1::PluginPtr Create(const nlohmann::json& config, V1::Context& ctx);
	
private:
	void OnTimer(V1::BrowserTab& tab);
	
	QString Randomize();
	
private:
	std::vector<QString> m_keywords, m_blacklist;
	Wait m_search;
	Wait m_result;
	
	std::mt19937 m_rand;
	std::uniform_int_distribution<std::size_t> m_search_count{1, 4};
};

} // end of namespace
