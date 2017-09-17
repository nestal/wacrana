/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

#include <QApplication>

#include <QtWebEngine/QtWebEngine>
#include <QtCore/QCommandLineParser>

#include "Configuration.hh"
#include "MainWindow.hh"
#include "Context.hpp"

#include <random>

using namespace wacrana;

class Context : public V1::Context
{
public:
	std::mt19937& RandomGenerator() override
	{
		return m_rand;
	}
private:
	std::mt19937 m_rand{std::random_device{}()};
};

// set environment variable "QT_LOGGING_TO_CONSOLE" to 1 to enable logging in stderr.
// when run by clion under, there is no TTY so these log message will not be shown,
// apparently they have gone to syslog.

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	
	Context ctx;
	Configuration config{"wacrana.json", ctx};
	QtWebEngine::initialize();
	
	MainWindow wnd{config};
	wnd.show();

	return app.exec();
}
