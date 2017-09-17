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
#include <QtCore/QJsonObject>
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

Beethoven::Beethoven(const std::vector<QString>& keywords, const Wait& search, const Wait& result) :
	m_keywords{keywords},
	m_search{search},
	m_result{result}
{
}

/**
 * \brief Callback when the plugin has been loaded.
 * \param config JSON object read from configuration file.
 *
 * This is the chance for a plugin to read its configuration. The JSON object that
 * specifies loading this plugin is passed as argument. Typically the "lib" field
 * has the path to the DSO/DLL that implements this plugin, and the "factory"
 * field is the symbol name of the factory function that creates this plugin.
 * This factory function should be the only symbol that is exported/made visible
 * by the DSO/DLL.
 */
Beethoven::Beethoven(const QJsonObject& config) :
	m_result{config["wait_time"].toObject()["search_result"].toObject()},
	m_search{config["wait_time"].toObject()["search"].toObject()}
{
	for (auto&& jval : config["keywords"].toArray())
		m_keywords.push_back(jval.toString());
	
	for (auto&& jval : config["blacklist"].toArray())
		m_blacklist.push_back(jval.toString());
	std::sort(m_blacklist.begin(), m_blacklist.end());
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
						{
							auto r = std::equal_range(m_blacklist.begin(), m_blacklist.end(), s);
							if (r.first == r.second)
								m_keywords.push_back(s);
						}
					}
				}
				
				std::sort(m_keywords.begin(), m_keywords.end());
				m_keywords.erase(std::unique(m_keywords.begin(), m_keywords.end()), m_keywords.end());
				qDebug() << "search result: " << m_keywords.size() << " keywords";
			});
			
			tab.SingleShotTimer(m_result.Random(m_rand), [this](V1::BrowserTab& tab){OnTimer(tab);});
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
	return std::make_unique<Beethoven>(m_keywords, m_search, m_result);
}

QString Beethoven::Randomize() const
{
	auto count = static_cast<std::size_t>(1 + qrand() % 4);
	
	// avoid infinite loop
	std::set<QString> result;
	if (count >= m_keywords.size())
		result.insert(m_keywords.begin(), m_keywords.end());
	
	while (result.size() < count)
		result.insert(m_keywords[qrand() % m_keywords.size()] + ' ');

	return std::accumulate(result.begin(), result.end(), QString{});
}

} // end of namespace

#include "ResourceLoader.hh"
WCAPI_RESOURCE_LOADER(Beethoven)

extern "C" WCAPI wacrana::V1::Plugin* Load(const QJsonObject& config)
{
	return new wacrana::Beethoven{config};
}
