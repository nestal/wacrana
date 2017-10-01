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

#include "ActivePersona.hh"

#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QMouseEvent>
#include <QStatusBar>

#include <stdexcept>

namespace wacrana {

BrowserTab::BrowserTab(QWidget *parent, double zoom) :
	QWidget{parent},
	m_ui{std::make_unique<Ui::BrowserTab>()},
	m_timer{new ProgressTimer{*this}}
{
	m_ui->setupUi(this);
	m_ui->m_page->setZoomFactor(zoom);
	
	connect(m_ui->m_page, &QWebEngineView::loadStarted,  this, &BrowserTab::OnLoadStarted);
	connect(m_ui->m_page, &QWebEngineView::loadFinished, this, &BrowserTab::OnLoadFinished);
	connect(m_ui->m_page, &QWebEngineView::iconChanged,  [this](const QIcon& icon)
	{
		Q_EMIT IconChanged(m_persona ? QIcon{QString::fromStdString(m_persona->Icon())} : icon);
	});
	
	connect(m_ui->m_page->page(), &QWebEnginePage::titleChanged, [this](const QString& title){Q_EMIT TitleChanged(title);});
	
	static const auto intervalMs = 500;
	startTimer(intervalMs);
}

BrowserTab::~BrowserTab() = default;

void BrowserTab::OnLoadFinished(bool ok)
{
	m_seqnum++;

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
	return m_persona ? QIcon{QString::fromStdString(m_persona->Icon())} : m_ui->m_page->icon();
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

void BrowserTab::InjectScript(const QString& javascript, ScriptCallback&& callback)
{
	m_ui->m_page->page()->runJavaScript(javascript, [cb=std::move(callback), this](const QVariant &v)
	{
		if (cb)
			cb(v, *this);
	});
}

void BrowserTab::InjectScriptFile(const QString& path)
{
	QFile script{path};
	if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
		throw std::runtime_error(script.errorString().toStdString());
	
	InjectScript(QString{script.readAll()}, {});
}

void BrowserTab::SingleShotTimer(TimeDuration timeout, TimerCallback&& callback)
{
	using namespace std::chrono;
	qDebug() << "waiting for " << duration_cast<duration<double, seconds::period>>(timeout).count() << " seconds";
	
	m_callback = std::move(callback);
	m_timer->Start(std::chrono::duration_cast<ProgressTimer::Duration>(timeout));
}

void BrowserTab::SetPersona(V1::PluginPtr&& persona)
{
	m_persona = std::make_unique<ActivePersona>(std::move(persona));
}

void BrowserTab::OnTimerUpdate(ProgressTimer::Duration remain)
{
	Q_EMIT WaitProgressUpdated(m_timer->Progress(), remain, m_timer->Total());
}

void BrowserTab::OnTimeout()
{
	if (m_persona)
		m_persona->Post(*this, [this](V1::BrowserTab& proxy){m_callback(proxy);});
	else
		m_callback(*this);
}

void BrowserTab::OnIdle()
{
	if (m_persona)
		m_persona->OnPageIdle(*this);
}

double BrowserTab::WaitProgress() const
{
	return m_timer->Progress();
}

void BrowserTab::ReportProgress(double percent)
{
	qDebug() << "progress = " << percent;
}

void BrowserTab::LeftClick(const QPoint& pos)
{
	auto click = [pos](QWidget *dest)
	{
        QCoreApplication::postEvent(dest, new QMouseEvent{
	        QEvent::MouseButtonPress,
			pos,
			Qt::LeftButton,
			Qt::MouseButton::NoButton,
			Qt::NoModifier
        });
	    // Some delay
	    QTimer::singleShot(100, [pos, dest]()
	    {
			QCoreApplication::postEvent(dest, new QMouseEvent{
				QEvent::MouseButtonRelease,
				pos,
				Qt::LeftButton,
				Qt::MouseButton::NoButton,
				Qt::NoModifier
			});
	    });
	};
	
	for (auto&& child : m_ui->m_page->children())
	{
		if (QWidget* wgt = qobject_cast<QWidget*>(child))
	    {
	        click(wgt);
	        break;
	    }
	}
}

void BrowserTab::timerEvent(QTimerEvent *event)
{
	m_timer->OnTimerInterval();
	QObject::timerEvent(event);
}

void BrowserTab::OnLoadStarted()
{
	// Cancel the timer and expect the persona will set it inside OnPageLoad().
	// Any timer function will be called only when the page that triggers them
	// is still on the browser.
	// In other words, if the user navigates aways from the page, the timer set
	// by a previous OnPageLoad() should not be triggered.
	if (m_persona)
		m_timer->Cancel();
}

std::size_t BrowserTab::SequenceNumber() const
{
	return m_seqnum;
}

void BrowserTab::Reseed(std::uint_fast32_t seed)
{
	if (m_persona)
		m_persona->OnReseed(seed);
}

} // end of namespace
