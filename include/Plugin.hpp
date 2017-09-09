/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#pragma once

#include <memory>

class QString;

namespace wacrana {
namespace V1 {

class MainWindow;
class BrowserTab;

/// \brief  Plugin interface. Implementation of plugins must implement this
class Plugin
{
public:
	virtual void OnPluginLoaded(MainWindow&) = 0;
	virtual void OnPageLoaded(MainWindow&, BrowserTab&) = 0;
	virtual void OnAction(MainWindow&, const QString& arg) = 0;
};

typedef std::unique_ptr<Plugin> (*Factory)();

}} // end of namespace
