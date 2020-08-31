#include <QString>
#include <albert/util/standardactions.h>

namespace KDEBrowserIntegration {

    struct SwitchToTabAction : public Core::StandardActionBase {
        public:
            SwitchToTabAction(const QString &text, QString service, int tabId) : 
                Core::StandardActionBase(text), service(service), tabId(tabId) {};
            void activate() override;

        private:
            QString service;
            int tabId;
    };

}