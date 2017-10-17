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

#include <functional>
#include <chrono>
#include <cstddef>
#include <memory>

class QUrl;
class QIcon;
class QString;
class QVariant;
class QPoint;

namespace wacrana {
namespace V1 {

class BrowserTab
{
protected:
	~BrowserTab() = default;
	
public:
	using TimeDuration  = std::chrono::system_clock::duration;
	using TimerCallback = std::function<void(V1::BrowserTab&)>;
	using ScriptCallback = std::function<void(const QVariant&, V1::BrowserTab&)>;
	
public:
	virtual void Load(const QUrl& url) = 0;
	virtual QUrl Location() const = 0;
	virtual QString Title() const = 0;
	
	virtual std::weak_ptr<BrowserTab> WeakFromThis() = 0;
	virtual std::weak_ptr<const BrowserTab> WeakFromThis() const = 0;
	
	// script injection
	virtual void InjectScript(const QString& javascript, ScriptCallback&& callback) = 0;
	virtual void InjectScriptFile(const QString& path) = 0;
	
	virtual void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) = 0;
	virtual void ReportProgress(double percent) = 0;
	virtual std::size_t SequenceNumber() const = 0;
};

}} // end of namespace
