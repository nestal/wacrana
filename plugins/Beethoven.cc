/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 9/12/17.
//

#include "Beethoven.hh"

#include "BrowserTab.hpp"

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QFile>

namespace wacrana {

QString Beethoven::Name() const
{
	return "Beethoven";
}

QString Beethoven::Version() const
{
	return "1.0";
}

void Beethoven::OnPluginLoaded(const QJsonObject&)
{
}

void Beethoven::OnPageLoaded(V1::MainWindow&, V1::BrowserTab& tab, bool ok)
{
	auto loc = tab.Location();
	qDebug() << "beethoven working " << (ok ? "ok" : "oops") << " " << loc.fileName();
	
	if (loc.host().contains("google.com", Qt::CaseInsensitive) && loc.fileName() != "search")
	{
		QFile script{":/scripts/Google.js"};
		if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
			qDebug() << script.errorString();
			
		tab.InjectScript(QString{script.readAll()});
		
		tab.InjectScript(
			R"____(
			Search("I am Beethoven");
			document.title;
			)____"
		);
	}
}

void Beethoven::OnAction(V1::MainWindow&, const QString& arg)
{

}

extern "C" WCAPI V1::Plugin* Load()
{
	return new Beethoven;
}

} // end of namespace