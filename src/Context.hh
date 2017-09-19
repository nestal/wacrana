/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>
#include "Context.hpp"

#include <random>

namespace wacrana {

class Context : public QObject, public V1::Context
{
	Q_OBJECT

public:
	Context()
	{
		// reseed every hour
		startTimer(3600 * 1000, Qt::VeryCoarseTimer);
	}

	std::mt19937& RandomGenerator() override
	{
		return m_rand;
	}

	void timerEvent(QTimerEvent*)
	{
		m_rand.seed(m_dev());
	}

private:
	std::random_device m_dev;
	std::mt19937 m_rand{m_dev()};
};

} // end of namespace
