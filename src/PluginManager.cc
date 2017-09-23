/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/23/17.
//

#include "PluginManager.hh"

#include <QHash>
#include <QtCore/QLibrary>

namespace wacrana {

PluginManager::PluginManager(Context& ctx) :
	m_ctx{ctx}
{
}

std::size_t PluginManager::Hash::operator()(const QString& s) const
{
	return qHash(s);
}

void PluginManager::LoadPlugin(const QJsonObject& )
{
}

} // end of namespace
