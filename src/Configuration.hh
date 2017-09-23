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

#include <QObject>

#include "OwnedFuture.hh"
#include "Persona.hpp"

#include <functional>
#include <unordered_map>

class QString;
class QJsonObject;

namespace wacrana {

/**
 * \brief Application configuration.
 *
 * This class loads configuration in JSON file. It will also load the plugins
 * specified in the configuration. The plugins will be loaded asynchronously
 * to reduce startup time. After it finished loading, the Finish() signal
 * will be emitted. Since the configuration is loaded asynchronously in
 * a different thread, the Finish() signal is emitted by that thread and should
 * be connect()'ed using Qt::QueuedConnection. You can call GetResult() to
 * check if the configuration is loaded successfully.
 *
 * When the browser starts, it construct a Configuration object and Load()'s it
 * before initialing the Qt web engine. This allows the Qt web engine can be
 * initialized in parallel with the configurations. If the configuration takes
 * some time to finish loading, the browser can still be started. This design
 * can ensure that the browser can be started quickly.
 */
class Configuration : public QObject
{
	Q_OBJECT
	
public:
	Configuration(const QString& path, V1::Context& ctx);
	~Configuration() override;
	
	V1::Persona* HomePage();
	double DefaultZoom() const;
	
	void GetResult();
	
	V1::PersonaPtr MakePersona(const std::string& name) const;
	std::vector<std::string> Persona() const;
	
Q_SIGNALS:
	void PreFinish();
	
	/**
	 * \brief   Signal emitted after the configuration finishes loading
	 *          asynchronouosly.
	 *
	 * The constructor will load the configuration asynchronously, but this
	 * signal will not be emitted until the caller thread returns to the
	 * main loop. (The "caller" thread is the thread which calls the
	 * Configuration constructor). Therefore you must make sure this signal
	 * is connected to a slot before returning to the main loop, otherwise
	 * the signal will be missed and the slot will never be called.
	 */
	void Finish();

private:
	using PackedFactory = std::function<V1::PersonaPtr ()>;
	static PackedFactory LoadPlugin(const QJsonObject& config, V1::Context& ctx);
	
private:
	std::future<void> m_loaded;
	
	OwnedFuture<V1::PersonaPtr>  m_home_page;
	OwnedFuture<double>         m_default_zoom;
	
	OwnedFuture<std::unordered_map<std::string, PackedFactory>> m_persona;
};

} // end of namespace
