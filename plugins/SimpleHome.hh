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
	explicit SimpleHome(const QJsonObject& config);
	explicit SimpleHome(const QUrl& url);
	
	QString Name() const override;
	QString Version() const override;
	
	void OnPageLoaded(V1::BrowserTab&, bool) override ;
	void OnAction(V1::MainWindow&, const QString& arg) override ;
	QIcon Icon() const override;
	
	V1::PluginPtr New() const override;
	
private:
	QUrl    m_home{"http://localhost"};
};

} // end of namespace
