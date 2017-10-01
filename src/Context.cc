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

V1::PluginPtr Context::MakePersona(const QString& name)
{
	return m_config.Plugins().NewPersona(name.toStdString(), *this);
}

std::vector<QString> Context::Find(const QString& role) const
{
	std::vector<QString> result;
	for (auto&& s : m_config.Plugins().Find(role.toStdString()))
		result.push_back(QString::fromStdString(s));
	return result;
}

const Configuration& Context::Config() const
{
	return m_config;
}

}
