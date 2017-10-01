/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#pragma once

#include "Context.hpp"
#include "Configuration.hh"

#include <QObject>

#include <random>

namespace wacrana {

class Context : public QObject, public V1::Context
{
	Q_OBJECT

public:
	Context(const std::string& config);

	const Configuration& Config() const ;
	
	V1::PluginPtr MakePersona(const QString& name);
	std::vector<QString> Find(const QString& role) const;

	std::uint_fast32_t Random() override;
	
private:
	void timerEvent(QTimerEvent*) override;

private:
	std::random_device m_dev;
	std::mt19937 m_rand{m_dev()};
	
	Configuration   m_config;
};

} // end of namespace
