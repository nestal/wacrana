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

void SimpleHome::OnPageIdle(V1::BrowserTab& tab)
{
}

void SimpleHome::OnAction(V1::MainWindow& browser, const QString&)
{
	browser.Current().Load(m_home);
}

QIcon SimpleHome::Icon() const
{
	return {};
}

extern "C" WCAPI V1::Persona* Load(const QJsonObject& config, wacrana::V1::Context&)
{
	return new SimpleHome{config};
}

} // end of namespace
