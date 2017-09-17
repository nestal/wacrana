/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the webhama
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#pragma once

namespace wacrana {
namespace V1 {

class BrowserTab;

/**
 * \brief Interface for the plugins to interact with the main browser window
 *
 * This interface will be passed to some callbacks in the Plugin interfaces
 * to allow plugins to interact with the main browser window. For example,
 * plugins can call NewTab() to create a new tab in the main window.
 */
class MainWindow
{
protected:
	~MainWindow() = default;

public:
	virtual BrowserTab& Current() = 0;
	virtual BrowserTab& Tab(int index) = 0;
	virtual BrowserTab& NewTab() = 0;
	virtual int IndexOf(const BrowserTab& tab) const = 0;
	virtual int TabCount() const = 0;
};

}} // end of namespaces
