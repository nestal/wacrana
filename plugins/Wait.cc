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

#include <algorithm>

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
	auto sec = std::clamp(m_range(gen), m_min, m_max);

	using namespace std::chrono;
	return round<system_clock::duration>(duration<double, seconds::period>{sec});
}

} // end of namespace
