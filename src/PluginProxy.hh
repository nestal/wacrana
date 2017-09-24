/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/24/17.
//

#pragma once

#include "Persona.hpp"
#include "GeneralPlugin.hpp"
#include "MainWindow.hpp"
#include "BrowserTab.hpp"

#include <boost/asio.hpp>

namespace wacrana {

class PersonaProxy : public V1::Persona
{
public:
	PersonaProxy(boost::asio::io_service& ios, V1::PersonaPtr&& adaptee, V1::BrowserTab& parent);
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override;
	void OnPageIdle(V1::BrowserTab& tab) override;
	QIcon Icon() const override;

private:
	boost::asio::io_service& m_ios;
	V1::PersonaPtr m_adaptee;
};

} // end of namespace
