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

class ProgressTimer : public QObject
{
	Q_OBJECT
	
public:
	using Duration = std::chrono::steady_clock::duration;
	using TimePoint = std::chrono::steady_clock::time_point;

public:
	explicit ProgressTimer(QObject *parent, Duration idle = std::chrono::duration_cast<Duration>(std::chrono::seconds{30}));
	
	void Start(Duration timeout);
	Duration Remains() const;
	double Progress() const;
	
Q_SIGNALS:
	void Update(Duration remains);
	void Timeout();
	void OnIdle();
	
private:
	bool m_is_idle{true};
	
    void timerEvent(QTimerEvent *event) override;
	
	TimePoint m_start, m_deadline;
	Duration m_idle;
};

} // end of namespace
