/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/25/17.
//

#pragma once

#include <chrono>

namespace wacrana {

class TimerEventCallback
{
public:
	using Clock = std::chrono::steady_clock;

protected:
	~TimerEventCallback() = default;

public:
	virtual void OnTimerUpdate(Clock::duration remains) = 0;
	virtual void OnTimeout() = 0;
	virtual void OnIdle() = 0;
};

} // end of namespace
