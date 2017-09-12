#include <QApplication>
#include <QtWebEngine/QtWebEngine>
#include <QtWidgets/QMessageBox>

#include "Configuration.hh"
#include "MainWindow.hh"

using namespace wacrana;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);

	Configuration config;
	Q_ASSERT(config.thread() == QThread::currentThread());
	
	// Must connect the signal before calling Configuration::Load(), otherwise the signal
	// may be missed.
	QObject::connect(&config, &Configuration::Finish, &app, [&config]
	{
		Q_ASSERT(QThread::currentThread() == config.thread());
		try
		{
			config.GetResult();
		}
		catch (std::exception& e)
		{
			QMessageBox::critical(nullptr, QObject::tr("Configuration Error"), e.what());
		}
		catch (...)
		{
			QMessageBox::critical(nullptr, QObject::tr("Configuration Error"), "Unknown exception");
		}
	}, Qt::QueuedConnection);
	config.Load("wacrana.json");
	
	QtWebEngine::initialize();
	
	MainWindow wnd{config};
	wnd.show();

	return app.exec();
}
