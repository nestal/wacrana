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
class QJsonValue;

namespace wacrana {

class Wait
{
public:
	Wait(const QJsonValue& config);
	
	std::chrono::system_clock::duration Random() const;
	
private:
	int m_minMs{1000}, m_maxMs{30000};
};

} // end of namespace
