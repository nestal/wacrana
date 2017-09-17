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

#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QJsonArray>
#include <set>

namespace wacrana {

Beethoven::~Beethoven() = default;

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

void Beethoven::OnPageLoaded(V1::BrowserTab& tab, bool ok)
{
	auto loc = tab.Location();
	if (loc.host().contains("google.com", Qt::CaseInsensitive))
	{
		if (loc.fileName() == "search")
		{
			tab.InjectScriptFile(":/scripts/Google.js");
			tab.InjectScript("Google.RelatedWords();", [this](const QVariant& terms)
			{
				for (auto&& var : terms.toList())
				{
					for (auto&& term : var.toString().split("\n", QString::SkipEmptyParts))
					{
						for (auto&& s : term.split(" ", QString::SkipEmptyParts))
							if (!s.contains("搜尋"))
								m_keywords.push_back(s);
					}
				}
				
				std::sort(m_keywords.begin(), m_keywords.end());
				m_keywords.erase(std::unique(m_keywords.begin(), m_keywords.end()), m_keywords.end());
				qDebug() << "search result: " << m_keywords.size() << " keywords";
			});
			
			tab.SingleShotTimer(5000, [this](V1::BrowserTab& tab){OnTimer(tab);});
		}
		else if (ok)
		{
			tab.InjectScriptFile(":/scripts/Google.js");
			tab.InjectScriptFile(":/scripts/Beethoven.js");
			tab.InjectScript("Beethoven('" + Randomize() + "');", {});
		}
	}
	else if (loc.url() == "about:blank")
		OnTimer(tab);
}

void Beethoven::OnAction(V1::MainWindow&, const QString& arg)
{
}

void Beethoven::OnTimer(V1::BrowserTab& tab)
{
	qDebug() << "going back to google";
	tab.Load({"https://google.com"});
}

QIcon Beethoven::Icon() const
{
	return m_icon;
}

V1::PluginPtr Beethoven::New() const
{
	return std::make_unique<Beethoven>();
}

QString Beethoven::Randomize() const
{
	std::set<QString> result;
	
	auto count = 1 + qrand() % 4;
	while (result.size() < count)
		result.insert(m_keywords[qrand() % m_keywords.size()] + ' ');

	return std::accumulate(result.begin(), result.end(), QString{});
}

} // end of namespace

#include "ResourceLoader.hh"
WCAPI_RESOURCE_LOADER(Beethoven)

extern "C" WCAPI wacrana::V1::Plugin* Load()
{
	return new wacrana::Beethoven;
}
