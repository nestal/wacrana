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

Wait::Wait(const QJsonObject& config) :
	m_range{
		config["mean"].toDouble(),
		config["stddev"].toDouble()
	},
	m_min{config["min"].toDouble()},
	m_max{config["max"].toDouble()}
{
}

std::chrono::system_clock::duration Wait::Random(std::mt19937& gen)
{
	using namespace std::chrono;
	using SecondsF = duration<double, seconds::period>;
	auto value = m_range(gen);
	value = value > m_max ? m_max : (value < m_min ? m_min : value);
	return duration_cast<system_clock::duration>(SecondsF{value});
}

} // end of namespace
