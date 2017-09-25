/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <functional>

namespace wacrana {
namespace V1 {

class BrowserTab;

class Timer
{
public:
	using TimeDuration  = std::chrono::system_clock::duration;
	using TimerCallback = std::function<void(V1::BrowserTab&)>;

	virtual ~Timer() = default;

	virtual void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) = 0;
} ;

}} // end of namespace
