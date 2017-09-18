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

namespace wacrana {

namespace {
const std::chrono::milliseconds interval{100};
}

ProgressTimer::ProgressTimer(QObject *parent, wacrana::ProgressTimer::Duration idle) :
	QObject{parent},
	m_idle{idle}
{
	startTimer(interval.count());
}

void ProgressTimer::timerEvent(QTimerEvent *event)
{
	using namespace std::chrono_literals;
	
	if (m_remains > interval)
	{
		m_remains -= interval;
		if (!m_is_idle)
			Q_EMIT Update(m_remains);
	}
	else if (m_is_idle)
	{
		m_remains = Duration::zero();
		Q_EMIT OnIdle();
		
		m_remains = m_idle;
	}
	else
	{
		m_remains = Duration::zero();
		Q_EMIT Timeout();
		
		m_is_idle = true;
		m_remains = m_idle;
	}
	
	QObject::timerEvent(event);
}

void ProgressTimer::Start(ProgressTimer::Duration timeout)
{
	m_timeout = m_remains = timeout;
	m_is_idle = false;
}

ProgressTimer::Duration ProgressTimer::Remains() const
{
	return m_remains;
}

double ProgressTimer::Progress() const
{
	return m_timeout.count() > 0 ? static_cast<double>(m_remains.count()) / m_timeout.count() : 0.0;
}

} // end of namespace
