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

#include "Wait.hh"

#include <QtGui/QIcon>

#include <memory>
#include <random>

namespace wacrana {

class Beethoven : public QObject, public V1::Plugin
{
	Q_OBJECT
	
public:
	explicit Beethoven(const QJsonObject& config);
	Beethoven(const std::vector<QString>& keywords, const Wait& search, const Wait& result);
	~Beethoven();
	
	QString Name() const override;
	QString Version() const override;
	
	void OnPageLoaded(V1::BrowserTab&, bool) override ;
	void OnAction(V1::MainWindow&, const QString& arg) override ;
	QIcon Icon() const override;
	
	V1::PluginPtr New() const override;

private:
	void OnTimer(V1::BrowserTab& tab);
	
	QString Randomize() const;
	
private:
	QIcon m_icon{":/icon/Beethoven.jpg"};
	
	std::vector<QString> m_keywords, m_blacklist;
	Wait m_search;
	Wait m_result;
	
	std::mt19937 m_rand{std::random_device{}()};
};

} // end of namespace
