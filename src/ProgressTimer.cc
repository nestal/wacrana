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

#include <QDebug>

namespace wacrana {

namespace {
const std::chrono::milliseconds interval{500};
}

ProgressTimer::ProgressTimer(QObject *parent, wacrana::ProgressTimer::Duration idle) :
	QObject{parent},
	m_idle{idle},
	m_deadline{std::chrono::steady_clock::now() + m_idle}
{
	startTimer(interval.count());
}

void ProgressTimer::timerEvent(QTimerEvent *event)
{
	using namespace std::chrono_literals;
	
	auto now = std::chrono::steady_clock::now();
	if (now < m_deadline)
	{
		if (!m_is_idle)
			Q_EMIT Update(m_deadline - now);
	}
	else if (m_is_idle)
	{
		Q_EMIT OnIdle();
		m_deadline += m_idle;
	}
	else
	{
		Q_EMIT Timeout();
		
		m_is_idle = true;
		m_deadline += m_idle;
	}
	
	QObject::timerEvent(event);
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
	return static_cast<double>((m_deadline - std::chrono::steady_clock::now()).count()) /
		(m_deadline - m_start).count();
}

} // end of namespace
