/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#pragma once

#include "Plugin.hpp"

#include <QtCore/QUrl>

namespace wacrana {

class SimpleHome : public V1::Plugin
{
public:
	explicit SimpleHome(const nlohmann::json& config);
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override;
	void OnPageIdle(V1::BrowserTab& tab) override;
	std::string Icon() const override;
	void OnReseed(std::uint_fast32_t seed) override;
	
	static V1::PluginPtr Create(const nlohmann::json& config, V1::Context& ctx);
	
private:
	QUrl    m_home{"http://localhost"};
};

} // end of namespace
