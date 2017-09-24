/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/18/17.
//

#pragma once

#include <QObject>

#include <chrono>
#include <functional>

class QTimerEvent;

namespace wacrana {

class TimerEventCallback;

class ProgressTimer : public QObject
{
	Q_OBJECT
	
public:
	using Duration = std::chrono::steady_clock::duration;
	using TimePoint = std::chrono::steady_clock::time_point;

public:
	explicit ProgressTimer(QObject *parent, TimerEventCallback& callback,
		Duration idle = std::chrono::duration_cast<Duration>(std::chrono::seconds{30}));
	
	void Start(Duration timeout);
	Duration Remains() const;
	Duration Total() const;
	double Progress() const;
	
private:
	bool m_is_idle{true};
	
    void timerEvent(QTimerEvent *event) override;

	Duration m_idle;
	TimePoint m_start, m_deadline;
	
	TimerEventCallback& m_callback;
};

} // end of namespace
