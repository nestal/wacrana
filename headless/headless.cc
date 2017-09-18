/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Thomson Reuters Limited
// This document contains information proprietary to Thomson Reuters Limited, and
// may not be reproduced, disclosed or used in whole or in part without
// the express written permission of Thomson Reuters Limited.
//
/////////////////////////////////////////////////////////////////////////

#include <QtWebEngine/QtWebEngine>
#include <QWebEnginePage>
#include <QEventLoop>
#include <QApplication>
#include <iostream>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QtWebEngine::initialize();

	QString filename = "output.pdf";
	QWebEnginePage page;

	app.connect(&page, &QWebEnginePage::loadFinished, [&page, &app, filename]()
    {
        page.runJavaScript("document.title;", [&app](const QVariant& var)
        {
	        std::cout << "title is: " << var.toString().toStdString() << std::endl;
            app.exit();
        });
    });
    page.load({"https://google.com"});
    return app.exec();
}
