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
#include <chrono>

namespace wacrana {

Wait::Wait(const nlohmann::json& config) :
	m_range{config["mean"], config["stddev"]},
	m_min{config["min"]},
	m_max{config["max"]}
{
}

std::chrono::system_clock::duration Wait::Random(std::mt19937& gen)
{
	auto sec = m_range(gen);
	sec = (sec < m_min ? m_min : (sec > m_max ? m_max : sec));

	using namespace std::chrono;
	return duration_cast<system_clock::duration>(duration<double, seconds::period>{sec});
}

void from_json(const nlohmann::json& config, Wait& wait)
{
	wait.m_range = std::normal_distribution<>{config["mean"], config["stddev"]};
	wait.m_min   = config["min"];
	wait.m_max   = config["max"];
}

} // end of namespace
