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

namespace WebHama {
namespace V1 {

class BrowserTab;

class MainWindow
{
public:
	virtual BrowserTab* Current() = 0;
	virtual BrowserTab* Tab(int index) = 0;
	virtual BrowserTab* NewTab() = 0;
};

}} // end of namespaces
