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

namespace wacrana {

void SimpleHome::OnPluginLoaded(const QJsonObject& config)
{
	m_home = config["homepage"].toString();
}

void SimpleHome::OnPageLoaded(V1::MainWindow&, V1::BrowserTab&)
{
}

void SimpleHome::OnAction(V1::MainWindow& browser, const QString& arg)
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

extern "C" WCAPI std::unique_ptr<V1::Plugin> Load()
{
	return std::make_unique<SimpleHome>();
}

} // end of namespace
