/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/18/17.
//

#include "ProgressTimer.hh"
#include "TimerEventCallback.hh"

#include <QDebug>

namespace wacrana {

ProgressTimer::ProgressTimer(TimerEventCallback& callback, wacrana::ProgressTimer::Duration idle) :
	m_idle{idle},
	m_deadline{std::chrono::steady_clock::now() + m_idle},
	m_callback{callback}
{
}

void ProgressTimer::OnTimerInterval()
{
	using namespace std::chrono_literals;
	
	auto now = std::chrono::steady_clock::now();
	if (now < m_deadline)
	{
		if (!m_is_idle)
			m_callback.OnTimerUpdate(Remains());
	}
	else if (m_is_idle)
	{
		m_callback.OnIdle();
		m_deadline += m_idle;
	}
	else
	{
		m_callback.OnTimeout();
		
		m_is_idle = true;
		m_deadline += m_idle;
	}
}

void ProgressTimer::Start(ProgressTimer::Duration timeout)
{
	m_start     = std::chrono::steady_clock::now();
	m_deadline  = m_start + timeout;
	m_is_idle   = false;
}

ProgressTimer::Duration ProgressTimer::Remains() const
{
	return m_deadline - std::chrono::steady_clock::now();
}

double ProgressTimer::Progress() const
{
	return static_cast<double>(Remains().count()) / Total().count();
}

ProgressTimer::Duration ProgressTimer::Total() const
{
	return m_deadline - m_start;
}

} // end of namespace
