#include <QDebug>
#include <QPointer>
#include <QRegularExpression>
#include <array>
#include <stdexcept>
#include <string>
#include <QMimeData>
#include <QUrl>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include "albert/util/standarditem.h"
#include "xdg/iconlookup.h"
#include "albert/util/standardactions.h"
#include "albert/util/standarditem.h"
#include "configwidget.h"
#include "extension.h"
#include "SwitchToTabAction.h"
Q_LOGGING_CATEGORY(qlc, "apps")
#define DEBG qCDebug(qlc,).noquote()
#define INFO qCInfo(qlc,).noquote()
#define WARN qCWarning(qlc,).noquote()
#define CRIT qCCritical(qlc,).noquote()
using namespace Core;
using namespace std;

namespace {

    const uint NUMBROWSERS = 6;

    array<const QString, 6> browserNames{
        {"face-smirk",
        "google-chrome",
        "chromium-browser",
        "firefox",
        "opera",
        "vivaldi"}
    };

}

class KDEBrowserIntegration::Private {
    public:
        QPointer<ConfigWidget> widget;
        QHash<QString, QString> iconPaths;
        vector<QString> commands;
        QHash<QString /*dbus service name*/, QString /*browser (chrome, firefox, ..)*/> m_serviceToBrowser;

};


/** ***************************************************************************/
KDEBrowserIntegration::Extension::Extension()
    : Core::Extension("org.albert.extension.kdebrowserintegration"),
      Core::QueryHandler(Core::Plugin::id()),
      d(new Private) {

    registerQueryHandler(this);

    // Load settings
    QString currentIconPath;
    for (size_t i = 0; i < NUMBROWSERS; ++i) {
        currentIconPath = XDG::IconLookup::iconPath(browserNames[i]);
        currentIconPath = currentIconPath == "" ? "system-settings" : currentIconPath;
        d->iconPaths.insert(browserNames[i], currentIconPath);
    }
}



/** ***************************************************************************/
KDEBrowserIntegration::Extension::~Extension() {

}


// Will handle user configuration of extension in albert settings menu
/** ***************************************************************************/
QWidget *KDEBrowserIntegration::Extension::widget(QWidget *parent) {
    if (d->widget.isNull()) {
        d->widget = new ConfigWidget(parent);
    }
    return d->widget;
}

/** ***************************************************************************/
void KDEBrowserIntegration::Extension::handleQuery(Core::Query * query) const {

    if ( query->string().isEmpty())
        return;

    const QString &term = query->string();

    // first look for all running hosts, there can be multiple browsers running
    QDBusReply<QStringList> servicesReply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
    QStringList services;
    if (servicesReply.isValid()) {
        services = servicesReply.value();
    }

    for (const QString &service: services) {
        if (!service.startsWith(QLatin1String("org.kde.plasma.browser_integration"))) {
            continue;
        }

        QString browser = d->m_serviceToBrowser.value(service);
        if (browser.isEmpty()) { // now ask what browser we're dealing with
            // FIXME can we use our dbus xml for this?
            QDBusMessage message = QDBusMessage::createMethodCall(service,
                                               QStringLiteral("/Settings"),
                                               QStringLiteral("org.freedesktop.DBus.Properties"),
                                               QStringLiteral("Get"));
            message.setArguments({
                QStringLiteral("org.kde.plasma.browser_integration.Settings"),
                QStringLiteral("Environment")
            });

            QDBusMessage reply = QDBusConnection::sessionBus().call(message);

            if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().count() != 1) {
                continue;
            }

            // what a long tail of calls...
            browser = reply.arguments().at(0).value<QDBusVariant>().variant().toString();
            d->m_serviceToBrowser.insert(service, browser);
        }

        QDBusMessage message =
            QDBusMessage::createMethodCall(service,
                                           QStringLiteral("/TabsRunner"),
                                           QStringLiteral("org.kde.plasma.browser_integration.TabsRunner"),
                                           QStringLiteral("GetTabs")
            );

        QDBusMessage reply = QDBusConnection::sessionBus().call(message);

        if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().length() != 1) {
            continue;
        }

        auto arg = reply.arguments().at(0).value<QDBusArgument>();
        auto tabvs = qdbus_cast<QList<QVariant>>(arg);

        for (const QVariant &tabv : tabvs)
        {

            auto tab = qdbus_cast<QVariantHash>(tabv.value<QDBusArgument>());

            // add browser name or window name or so to it maybe?
            const QString &text = tab.value(QStringLiteral("title")).toString();
            if (text.isEmpty()) { // shouldn't happen?
                continue;
            }

            // will be used to raise the tab eventually
            int tabId = tab.value(QStringLiteral("id")).toInt();
            if (!tabId) {
                continue;
            }

            const QUrl url(tab.value(QStringLiteral("url")).toString());
            if (!url.isValid()) {
                continue;
            }

            const bool incognito = tab.value(QStringLiteral("incognito")).toBool();

            auto item = make_shared<StandardItem>(text);
            item->setText(text);
            item->setSubtext(url.toDisplayString());

            qreal relevance = 0;

            // someone was really busy here, typing the *exact* title or url :D
            if (text.compare(term, Qt::CaseInsensitive) == 0 || url.toString().compare(term, Qt::CaseInsensitive) == 0) {
                relevance = 1;
            } else {

                if (text.contains(term, Qt::CaseInsensitive)) {
                    relevance = 0.9;
                    if (text.startsWith(term, Qt::CaseInsensitive)) {
                        relevance += 0.1;
                    }
                } else if (url.host().contains(term, Qt::CaseInsensitive)) {
                    relevance = 0.7;
                    if (url.host().startsWith(term, Qt::CaseInsensitive)) {
                        relevance += 0.1;
                    }
                } else if (url.path().contains(term, Qt::CaseInsensitive)) {
                    relevance = 0.5;
                    if (url.path().startsWith(term, Qt::CaseInsensitive)) {
                        relevance += 0.1;
                    }
                }
            }

            if (!relevance) {
                continue;
            }

            QString iconName;

            if (incognito) {
                iconName = QStringLiteral("face-smirk");// TODO QStringLiteral("incognito");
            } else if (browser == QLatin1String("chrome")) {
                iconName = QStringLiteral("google-chrome");
            } else if (browser == QLatin1String("chromium")) {
                iconName = QStringLiteral("chromium-browser");
            } else if (browser == QLatin1String("firefox")) {
                iconName = QStringLiteral("firefox");
            } else if (browser == QLatin1String("opera")) {
                iconName = QStringLiteral("opera");
            } else if (browser == QLatin1String("vivaldi")) {
                iconName = QStringLiteral("vivaldi");
            }

            item->setIconPath(d->iconPaths.value(iconName));

            item->addAction(make_shared<SwitchToTabAction>(text, service, tabId));

            query->addMatch(std::move(item), static_cast<uint>(static_cast<float>(query->string().size())/text.size()*UINT_MAX));
        }
    }
}