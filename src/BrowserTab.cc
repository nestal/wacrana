/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the webhama
    distribution for more details.
*/

//
// Created by nestal on 9/9/17.
//

#include "BrowserTab.hh"

namespace wacrana {

BrowserTab::BrowserTab(QWidget *parent) : QWidget{parent}
{
	m_ui.setupUi(this);
	
	connect(m_ui.m_page, &QWebEngineView::loadFinished, this, &BrowserTab::OnLoad);
	connect(m_ui.m_page, &QWebEngineView::iconChanged,  [this](const QIcon& icon){Q_EMIT IconChanged(icon);});
}

void BrowserTab::OnLoad(bool val)
{
	setWindowTitle(m_ui.m_page->title());
	Q_EMIT LoadFinished(val);
}

void BrowserTab::Load(const QUrl& url)
{
	m_ui.m_page->load(url);
}

QUrl BrowserTab::Location() const
{
	return m_ui.m_page->url();
}

QString BrowserTab::Title() const
{
	return m_ui.m_page->title();
}

QIcon BrowserTab::Icon() const
{
	return m_ui.m_page->icon();
}

void BrowserTab::Back()
{
	m_ui.m_page->back();
}

} // end of namespace
