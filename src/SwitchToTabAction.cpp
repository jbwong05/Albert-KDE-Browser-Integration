#include <QDebug>
#include <QString>
#include <QDBusMessage>
#include <QDBusConnection>
#include "SwitchToTabAction.h"

void KDEBrowserIntegration::SwitchToTabAction::activate() {
    QDBusMessage message = QDBusMessage::createMethodCall(service,
            QStringLiteral("/TabsRunner"),
            QStringLiteral("org.kde.plasma.browser_integration.TabsRunner"),
            QStringLiteral("Activate"));
    message.setArguments({tabId});
    QDBusConnection::sessionBus().call(message); // asyncCall?
}