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
#include "BrowserTab.hpp"

#include <QtCore/QUrl>
#include <QtCore/QString>

#include <boost/asio.hpp>
#include <thread>

namespace wacrana {

class ActivePersona : public V1::Persona
{
public:
	explicit ActivePersona(V1::PersonaPtr&& adaptee);
	~ActivePersona();
	
	void OnPageLoaded(V1::BrowserTab& tab, bool ok) override;
	void OnPageIdle(V1::BrowserTab& tab) override;
	QIcon Icon() const override;

	void Post(V1::BrowserTab& tab, std::function<void (V1::BrowserTab& proxy)>&& callback);
	
private:
	class BrowserTabProxy;
	
private:
	V1::PersonaPtr                  m_adaptee;
	boost::asio::io_service         m_ios;
	boost::asio::io_service::work   m_work;
	std::thread                     m_thread;
};

} // end of namespace
