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

#include "Persona.hpp"

#include <QtCore/QUrl>

namespace wacrana {

class SimpleHome : public V1::Persona
{
public:
	explicit SimpleHome(const QJsonObject& config);
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override;
	void OnPageIdle(V1::BrowserTab& tab) override;
	QIcon Icon() const override;
	
	static V1::PersonaPtr Create(const QJsonObject& config, V1::Context& ctx);
	
private:
	QUrl    m_home{"http://localhost"};
};

} // end of namespace
