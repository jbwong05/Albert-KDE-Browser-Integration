#pragma once
#include <QObject>
#include <QLoggingCategory>
#include <memory>
#include <string>
#include <QDBusMessage>
#include "albert/extension.h"
#include "albert/queryhandler.h"
Q_DECLARE_LOGGING_CATEGORY(qlc)

using namespace std;

namespace KDEBrowserIntegration {

    class Private;

    class Extension final :
        public Core::Extension, 
        public Core::QueryHandler {
        
        Q_OBJECT
        Q_PLUGIN_METADATA(IID ALBERT_EXTENSION_IID FILE "metadata.json")

        public:

            Extension();
            ~Extension() override;

            QString name() const override { return "KDE Browser Integration"; }
            QWidget *widget(QWidget *parent = nullptr) override;
            void handleQuery(Core::Query * query) const override;

        private:

            std::unique_ptr<Private> d;

    };
}
