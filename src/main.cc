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
#include "Context.hh"

#include <QtWidgets/QMessageBox>

using namespace wacrana;

// set environment variable "QT_LOGGING_TO_CONSOLE" to 1 to enable logging in stderr.
// when run by clion under, there is no TTY so these log message will not be shown,
// apparently they have gone to syslog.

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	
	try
	{
		Context ctx;
		Configuration config{"wacrana.json", ctx};
		QtWebEngine::initialize();
		
		MainWindow wnd{config};
		wnd.show();
		
		return app.exec();
	}
	catch (std::exception& e)
	{
		QMessageBox::critical(nullptr, QObject::tr("Exception"), QString::fromUtf8(e.what()));
		return -1;
	}
}
