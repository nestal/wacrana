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

#include <QtCore/QJsonObject>
#include <QtGui/QIcon>

namespace wacrana {

SimpleHome::SimpleHome(const QJsonObject& config) :
	m_home{config["homepage"].toString()}
{
}

void SimpleHome::OnPageLoaded(V1::BrowserTab&, bool)
{
}

void SimpleHome::OnAction(V1::MainWindow& browser, const QString&)
{
	browser.Current().Load(m_home);
}

QString SimpleHome::Name() const
{
	return "Simple Home";
}

QString SimpleHome::Version() const
{
	return "1.0";
}

QIcon SimpleHome::Icon() const
{
	return {};
}

V1::PluginPtr SimpleHome::New() const
{
	return std::make_unique<SimpleHome>(m_home);
}

SimpleHome::SimpleHome(const QUrl& url) : m_home{url}
{
}

extern "C" WCAPI V1::Plugin* Load(const QJsonObject& config, wacrana::V1::Context&)
{
	return new SimpleHome{config};
}

} // end of namespace
