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
#include "Context.hpp"

#include <QtCore/QDebug>
#include <QtCore/QUrl>

#include <set>
#include <thread>

namespace wacrana {

Beethoven::~Beethoven() = default;

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
Beethoven::Beethoven(const nlohmann::json& config, std::mt19937::result_type seed) :
	m_result{config["wait_time"]["search_result"]},
	m_search{config["wait_time"]["search"]},
	m_rand{seed}
{
	for (auto&& jval : config["keywords"])
		m_keywords.push_back(QString::fromStdString(jval.get<std::string>()));
	
	for (auto&& jval : config["blacklist"])
		m_blacklist.push_back(QString::fromStdString(jval.get<std::string>()));
	std::sort(m_blacklist.begin(), m_blacklist.end());
}

void Beethoven::OnPageLoaded(V1::BrowserTab& tab, bool ok)
{
	auto loc = tab.Location();
	if (loc.host().contains("google.com", Qt::CaseInsensitive))
	{
		if (loc.fileName() == "search")
		{
			auto tid = std::this_thread::get_id();
			
			tab.InjectScriptFile(":/scripts/Google.js");
			tab.InjectScript("Google.RelatedWords();").then([this, tid](auto fut_terms)
			{
				assert(tid == std::this_thread::get_id());
				for (auto&& var : fut_terms.get().toList())
				{
					for (auto&& term : var.toString().split("\n", QString::SkipEmptyParts))
					{
						auto wanted = 0;
						std::vector<QString> keywords;
						for (auto&& s : term.split(" ", QString::SkipEmptyParts))
						{
							if (s.size() <= 2)
								continue;
							
							auto r = std::equal_range(m_blacklist.begin(), m_blacklist.end(), s);
							if (r.first != r.second)
								continue;
							
							auto r2 = std::equal_range(m_keywords.begin(), m_keywords.end(), s);
							if (r2.first != r2.second)
								wanted++;
							else
								keywords.push_back(std::move(s));
						}

						qDebug() << wanted << " keywords found in related term";
						
						if (wanted > 1)
							for (auto&& k : keywords)
							{
								qDebug() << "adding keyword: " << k;
								m_keywords.push_back(std::move(k));
							}
					}
				}
				
				std::sort(m_keywords.begin(), m_keywords.end());
				m_keywords.erase(std::unique(m_keywords.begin(), m_keywords.end()), m_keywords.end());
				qDebug() << "search result: " << m_keywords.size() << " keywords";
			}, tab.Executor());
			tab.InjectScript("Google.SearchResults();").then([this, wtab=tab.WeakFromThis()](auto fut_results)
			{
				for (auto&& result : fut_results.get().toList())
				{
					auto&& map = result.toMap();
//					qDebug() << map["href"].toString() << ": " << map["text"].toString() << " (" << map["top"].toInt() << ", " << map["left"].toInt() << ")";
					
//					QRect rect{map["left"].toInt(), map["top"].toInt(), map["width"].toInt(), map["height"].toInt()};
//					qDebug() << map["href"].toString() << ": " << rect;
					auto url = map["href"].toString();


					auto tab = wtab.lock();
					if (!tab)
						break;
					
					tab->SingleShotTimer(m_result.Random(m_rand), [this, url, wtab=tab->WeakFromThis()]
					{
						if (auto tab = wtab.lock())
//				        tab.InjectScript("Google.IAmFeelingLucky();", {});
//						tab.LeftClick(rect.center());
							tab->InjectScript("Google.ClickSearchResult('" + url + "');");
					});
					break;
				}
			}, tab.Executor());
			
		}
		else if (ok)
		{
			tab.InjectScriptFile(":/scripts/Google.js");
			tab.InjectScriptFile(":/scripts/Beethoven.js");
			tab.InjectScript("Beethoven('" + Randomize() + "');");
		}
	}
	else if (loc.url() == "about:blank")
		OnTimer(tab);
	else
	{
		tab.SingleShotTimer(m_result.Random(m_rand), [this, wtab=tab.WeakFromThis()]
		{
			if (auto tab = wtab.lock())
				OnTimer(*tab);
		});
	}
}

void Beethoven::OnTimer(V1::BrowserTab& tab)
{
	qDebug() << "going back to google";
	tab.Load({"https://google.com"});
}

std::string Beethoven::Icon() const
{
	return ":/icon/Beethoven.jpg";
}

QString Beethoven::Randomize()
{
	auto count = m_search_count(m_rand);
	
	// avoid infinite loop
	std::set<QString> result;
	if (count >= m_keywords.size())
		result.insert(m_keywords.begin(), m_keywords.end());
	
	std::uniform_int_distribution<std::size_t> keyword_index{0, m_keywords.size()-1};
	while (result.size() < count)
		result.insert(m_keywords[keyword_index(m_rand)] + ' ');

	return std::accumulate(result.begin(), result.end(), QString{});
}

void Beethoven::OnPageIdle(V1::BrowserTab& tab)
{
	OnTimer(tab);
}

V1::PluginPtr Beethoven::Create(const nlohmann::json& config, V1::Context& ctx)
{
	return std::make_unique<Beethoven>(config, ctx.Random());
}

void Beethoven::OnReseed(std::uint_fast32_t seed)
{
	m_rand.seed(seed);
}

void Beethoven::OnAttachTab(V1::BrowserTab& tab)
{

}

void Beethoven::OnDetachTab(V1::BrowserTab& tab)
{

}

} // end of namespace
