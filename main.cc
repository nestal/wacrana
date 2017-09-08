#include <QApplication>
#include <QtWebEngine/QtWebEngine>

#include "MainWindow.hh"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);

	QtWebEngine::initialize();

	MainWindow wnd;
	wnd.show();

	return app.exec();
}
