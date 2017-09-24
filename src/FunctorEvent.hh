/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/24/17.
//

#pragma once

#include <QEvent>
#include <QtCore/QCoreApplication>

namespace wacrana {

template <typename Func>
class FunctorEvent : public QEvent
{
public:
	explicit FunctorEvent(Func&& func) : QEvent{QEvent::User}, m_func{std::move(func)}
	{
	}
	
	~FunctorEvent() override
	{
		try
		{
			m_func();
		}
		catch (...) {
		
		}
	}
	
private:
	Func m_func;
};

template <typename Func>
void PostMain(Func&& func, QObject *dest = qApp)
{
	QCoreApplication::postEvent(dest, new FunctorEvent<Func>(std::forward<Func>(func)));
}

} // end of namespace
