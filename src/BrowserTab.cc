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

#include "Plugin.hpp"

#include <QtCore/QFile>
#include <QtCore/QTimer>

#include <stdexcept>

namespace wacrana {

BrowserTab::BrowserTab(QWidget *parent, double zoom) :
	QWidget{parent},
	m_ui{std::make_unique<Ui::BrowserTab>()}
{
	m_ui->setupUi(this);
	m_ui->m_page->setZoomFactor(zoom);
	
	connect(m_ui->m_page, &QWebEngineView::loadFinished, this, &BrowserTab::OnLoad);
	connect(m_ui->m_page, &QWebEngineView::iconChanged,  [this](const QIcon& icon)
	{
		Q_EMIT IconChanged(m_persona ? m_persona->Icon() : icon);
	});
	
	connect(m_ui->m_page->page(), &QWebEnginePage::titleChanged, [this](const QString& title){Q_EMIT TitleChanged(title);});
}

BrowserTab::~BrowserTab() = default;

void BrowserTab::OnLoad(bool ok)
{
	if (ok)
		setWindowTitle(m_ui->m_page->title());
	
	if (m_persona)
		m_persona->OnPageLoaded(*this, ok);
	
	Q_EMIT LoadFinished(ok);
}

void BrowserTab::Load(const QUrl& url)
{
	m_ui->m_page->load(url);
}

QUrl BrowserTab::Location() const
{
	return m_ui->m_page->url();
}

QString BrowserTab::Title() const
{
	return m_ui->m_page->title();
}

QIcon BrowserTab::Icon() const
{
	return m_persona ? m_persona->Icon() : m_ui->m_page->icon();
}

void BrowserTab::Back()
{
	m_ui->m_page->back();
}

void BrowserTab::Forward()
{
	m_ui->m_page->forward();
}

QWebEnginePage *BrowserTab::Page()
{
	return m_ui->m_page->page();
}

double BrowserTab::ZoomFactor() const
{
	return m_ui->m_page->zoomFactor();
}

void BrowserTab::ZoomFactor(double zoom)
{
	m_ui->m_page->setZoomFactor(zoom);
}

void BrowserTab::Reload()
{
	m_ui->m_page->page()->triggerAction(QWebEnginePage::WebAction::Reload);
}

void BrowserTab::InjectScript(const QString& javascript, std::function<void(const QVariant&)>&& callback)
{
	m_ui->m_page->page()->runJavaScript(javascript, [cb=std::move(callback)](const QVariant &v)
	{
		qDebug() << "injected script result: " << v.toString();
		
		if (cb)
			cb(v);
	});
}

void BrowserTab::InjectScriptFile(const QString& path)
{
	QFile script{path};
	if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
		throw std::runtime_error(script.errorString().toStdString());
	
	InjectScript(QString{script.readAll()}, {});
}

void BrowserTab::SingleShotTimer(int msec, std::function<void(V1::BrowserTab&)>&& callback)
{
	QTimer::singleShot(msec, this, [this, cb=std::move(callback)]{cb(*this);});
}

void BrowserTab::SetPersona(const V1::Plugin& persona)
{
	m_persona = persona.New();
}

} // end of namespace
