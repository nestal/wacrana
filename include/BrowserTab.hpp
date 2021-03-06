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

#include "BrightFuture/BrightFuture.hh"

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
	using TimerCallback = std::function<void()>;
	
public:
	virtual void Load(const QUrl& url) = 0;
	virtual QUrl Location() const = 0;
	virtual QString Title() const = 0;
	
	virtual std::weak_ptr<BrowserTab> WeakFromThis() = 0;
	virtual std::weak_ptr<const BrowserTab> WeakFromThis() const = 0;
	virtual BrightFuture::Executor& Executor() = 0;
	
	// script injection
	virtual BrightFuture::future<QVariant> InjectScript(const QString& javascript) = 0;
	virtual BrightFuture::future<QVariant> InjectScriptFile(const QString& path) = 0;
	
	virtual void SingleShotTimer(TimeDuration timeout, TimerCallback&& callback) = 0;
	virtual void ReportProgress(double percent) = 0;
	virtual std::size_t SequenceNumber() const = 0;
};

}} // end of namespace
