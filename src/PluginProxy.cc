/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/24/17.
//

#include "PluginProxy.hh"
#include <QtGui/QIcon>

namespace wacrana {

PersonaProxy::PersonaProxy(boost::asio::io_service& ios, V1::PersonaPtr&& adaptee, V1::BrowserTab&) :
	m_ios{ios},
	m_adaptee{std::move(adaptee)}
{
}

void PersonaProxy::OnPageLoaded(V1::BrowserTab& tab, bool ok)
{
	m_ios.post([&tab, ok, this]{m_adaptee->OnPageLoaded(tab, ok);});
}

void PersonaProxy::OnPageIdle(V1::BrowserTab& tab)
{
	m_ios.post([&tab, this]{m_adaptee->OnPageIdle(tab);});
}

QIcon PersonaProxy::Icon() const
{
	// constant function should be thread-safe
	return m_adaptee->Icon();
}

} // end of namespace
