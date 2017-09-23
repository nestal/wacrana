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

#include "GeneralPlugin.hpp"

#include <QtCore/QUrl>

namespace wacrana {

class SimpleHome : public V1::GeneralPlugin
{
public:
	explicit SimpleHome(const QJsonObject& config);
	
	void OnAction(V1::MainWindow&, const QString& arg) override ;
	
private:
	QUrl    m_home{"http://localhost"};
};

} // end of namespace
