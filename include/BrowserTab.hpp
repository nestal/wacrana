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

class QUrl;
class QIcon;
class QString;

namespace wacrana {
namespace V1 {

class BrowserTab
{
protected:
	~BrowserTab() = default;
	
public:
	virtual void Load(const QUrl& url) = 0;
	virtual QIcon Icon() const = 0;
	virtual QUrl Location() const = 0;
	virtual QString Title() const = 0;
	virtual void Back() = 0;
	virtual void Forward() = 0;
	virtual double ZoomFactor() const = 0;
	virtual void ZoomFactor(double) = 0;
	virtual void Reload() = 0;
//	virtual void RunJavaScript(const QString& javascript) = 0;
};

}} // end of namespace
