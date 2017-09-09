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

namespace wacrana {

class SimpleHome : public V1::Plugin
{
public:
	QString Name() const override;
	QString Version() const override;
	
	void OnPluginLoaded(const QJsonObject&) override;
	void OnPageLoaded(V1::MainWindow&, V1::BrowserTab&) override ;
	void OnAction(V1::MainWindow&, const QString& arg) override ;
};

} // end of namespace
