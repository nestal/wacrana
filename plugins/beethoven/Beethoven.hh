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
#include "Persona.hpp"

#include "Wait.hh"

#include <QtGui/QIcon>

#include <memory>
#include <random>

namespace wacrana {

class Beethoven : public QObject, public V1::Persona
{
	Q_OBJECT
	
public:
	Beethoven(const QJsonObject& config, std::mt19937::result_type seed);
	~Beethoven() override;
	
	// copy and moves are not allowed. no one uses these anyway.
	Beethoven(const Beethoven&) = delete;
	Beethoven(Beethoven&&) = delete;
	Beethoven& operator=(const Beethoven&) = delete;
	Beethoven& operator=(Beethoven&&) = delete;
	
	void OnPageLoaded(V1::BrowserTab&, bool) override ;
	void OnPageIdle(V1::BrowserTab& tab) override;
	QIcon Icon() const override;

	static V1::PersonaPtr Create(const QJsonObject& config, V1::Context& ctx);
	
private:
	void OnTimer(V1::BrowserTab& tab);
	
	QString Randomize();
	
private:
	QIcon m_icon{":/icon/Beethoven.jpg"};
	
	std::vector<QString> m_keywords, m_blacklist;
	Wait m_search;
	Wait m_result;
	
	mutable std::mt19937 m_rand;
	std::uniform_int_distribution<std::size_t> m_search_count{1, 4};
};

} // end of namespace