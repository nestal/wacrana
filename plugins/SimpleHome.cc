/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#include "SimpleHome.hh"
#include "MainWindow.hpp"
#include "BrowserTab.hpp"

#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS

namespace wacrana {

SimpleHome::SimpleHome(const nlohmann::json& config) :
	m_home{QString::fromStdString(config["url"])}
{
}

void SimpleHome::OnPageLoaded(V1::BrowserTab& tab, bool)
{
	tab.Load(m_home);
}

void SimpleHome::OnPageIdle(V1::BrowserTab&)
{
}

V1::PluginPtr SimpleHome::Create(const nlohmann::json& config, V1::Context&)
{
	return std::make_unique<SimpleHome>(config);
}

std::string SimpleHome::Icon() const
{
	return {};
}

void SimpleHome::OnReseed(std::uint_fast32_t)
{
}

void SimpleHome::OnAttachTab(V1::BrowserTab& tab)
{

}

void SimpleHome::OnDetachTab(V1::BrowserTab& tab)
{

}

BOOST_DLL_ALIAS(
    wacrana::SimpleHome::Create, // <-- this function is exported with...
    Load                         // <-- ...this alias name
)

} // end of namespace
