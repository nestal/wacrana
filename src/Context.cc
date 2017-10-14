/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#include "Context.hh"

namespace wacrana {

Context::Context(const std::string& config) :
	m_config{config}
{
	// reseed every hour
	startTimer(3600 * 1000, Qt::VeryCoarseTimer);
}

void Context::timerEvent(QTimerEvent*)
{
	m_rand.seed(m_dev());
}

std::uint_fast32_t Context::Random()
{
	return m_rand();
}

const Configuration& Context::Config() const
{
	return m_config;
}

Configuration& Context::Config()
{
	return m_config;
}

}
