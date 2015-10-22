#ifndef PLATFORM_QT_HTTP_REQUEST_QT
#define PLATFORM_QT_HTTP_REQUEST_QT

#include <mbgl/storage/http_request_base.hpp>

#include <QNetworkRequest>
#include <QUrl>

class QNetworkReply;

namespace mbgl {

class HTTPQtContext;
class Response;

class HTTPQtRequest : public HTTPRequestBase
{
public:
    HTTPQtRequest(HTTPQtContext*, const Resource&, Callback);
    virtual ~HTTPQtRequest() = default;

    QUrl requestUrl() const;
    QNetworkRequest networkRequest() const;

    void handleNetworkReply(QNetworkReply *reply);

    // HTTPRequestBase implementation.
    void cancel() final;

private:
    HTTPQtContext* m_context;
};

} // namespace mbgl

#endif // PLATFORM_QT_HTTP_REQUEST_QT
