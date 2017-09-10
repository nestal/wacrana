#include <QApplication>
#include <QtWebEngine/QtWebEngine>
#include <QtWidgets/QMessageBox>

#include "MainWindow.hh"

using namespace wacrana;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);

	Configuration config;
	// Must connect the signal before calling Configuration::Load(), otherwise the signal
	// may be missed.
	app.connect(&config, &Configuration::Finish, &app, [&config, &app]
	{
		try
		{
			config.GetResult();
		}
		catch (std::exception& e)
		{
			QMessageBox::critical(nullptr, app.tr("Exception"), e.what());
		}
	}, Qt::QueuedConnection);
	config.Load("wacrana.json");
	
	QtWebEngine::initialize();
	
	MainWindow wnd{config};
	wnd.show();

	return app.exec();
}
