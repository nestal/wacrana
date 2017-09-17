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
#include <QDebug>

namespace wacrana {

Wait::Wait(const QJsonValue& config) :
	m_range{(
		config.toObject()["max"].toDouble(1) + config.toObject()["min"].toDouble(30)
	)/2.0, 10.000}
{
}

std::chrono::system_clock::duration Wait::Random(std::mt19937& gen)
{
	using namespace std::chrono;
	using SecondsF = duration<double, seconds::period>;
	return duration_cast<system_clock::duration>(SecondsF{m_range(gen)});
}

} // end of namespace
