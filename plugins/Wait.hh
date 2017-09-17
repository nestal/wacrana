/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/17/17.
//

#pragma once

#include <chrono>
#include <random>

class QJsonValue;

namespace wacrana {

class Wait
{
public:
	Wait(const QJsonValue& config);
	
	std::chrono::system_clock::duration Random(std::mt19937& gen);
	
private:
	std::normal_distribution<> m_range{20};
};

} // end of namespace
