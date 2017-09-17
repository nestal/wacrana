/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/17/17.
//

#include "Wait.hh"

#include <QJsonObject>

namespace wacrana {

Wait::Wait(const QJsonValue& config) :
	m_maxMs{config.toObject()["max"].toInt(1000)},
	m_minMs{config.toObject()["min"].toInt(30000)}
{
}

std::chrono::system_clock::duration Wait::Random() const
{
	using namespace std::chrono;
	
	auto ms = m_minMs + qrand() % (m_maxMs-m_minMs);
	return duration_cast<system_clock::duration>(milliseconds{ms});
}

} // end of namespace
