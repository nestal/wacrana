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

#include <QtCore/QObject>
#include "Plugin.hpp"

#include <QtGui/QIcon>

#include <memory>

namespace wacrana {

class Beethoven : public QObject, public V1::Plugin
{
	Q_OBJECT
	
public:
	Beethoven() = default;
	~Beethoven();
	
	QString Name() const override;
	QString Version() const override;
	
	void OnPluginLoaded(const QJsonObject&) override;
	void OnPageLoaded(V1::BrowserTab&, bool) override ;
	void OnAction(V1::MainWindow&, const QString& arg) override ;
	QIcon Icon() const override;
	
	V1::PluginPtr New() const override;

private:
	void OnTimer(V1::BrowserTab& tab);
	
	QString Randomize() const;
	
private:
	QIcon m_icon{":/icon/Beethoven.jpg"};
	
	std::vector<QString> m_keywords{
		"beethoven", "classical", "symphony", "bach", "mozart", "nocturne",
		"moonlight", "music", "fur elise"
	};
};

} // end of namespace
