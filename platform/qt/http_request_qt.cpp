#include "http_request_qt.hpp"

#include "http_context_qt.hpp"

#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/string.hpp>

#include <QByteArray>
#include <QNetworkReply>
#include <QPair>

namespace mbgl {

HTTPQtRequest::HTTPQtRequest(HTTPQtContext* context, const Resource& resource_, Callback callback)
    : HTTPRequestBase(resource_, callback)
    , m_context(context)
{
    m_context->request(this);
}

QUrl HTTPQtRequest::requestUrl() const
{
    return QUrl::fromPercentEncoding(QByteArray(resource.url.data(), resource.url.size()));
}

QNetworkRequest HTTPQtRequest::networkRequest() const
{
    QNetworkRequest req = QNetworkRequest(requestUrl());
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    req.setRawHeader("User-Agent", "MapboxGL/1.0 [Qt]");

    if (resource.priorEtag) {
        req.setRawHeader("If-None-Match", QByteArray(resource.priorEtag->data(), resource.priorEtag->size()));
    } else if (resource.priorModified) {
        req.setRawHeader("If-Modified-Since", util::rfc1123(*resource.priorModified).c_str());
    }

    return req;
}

void HTTPQtRequest::handleNetworkReply(QNetworkReply *reply)
{
    mbgl::Response response;

    using Error = Response::Error;

    // Handle non-HTTP errors (i.e. like connection).
    if (reply->error() && reply->error() < 100) {
        response.error = std::make_unique<Error>(
            Error::Reason::Connection, reply->errorString().toStdString());
        notify(response);
        delete this;

        return;
    }

    QPair<QByteArray, QByteArray> line;
    foreach(line, reply->rawHeaderPairs()) {
        QString header = QString(line.first).toLower();

        if (header == "last-modified") {
            response.modified = util::parseTimePoint(line.second.constData());
        } else if (header == "etag") {
            response.etag = std::string(line.second.constData(), line.second.size());
        } else if (header == "cache-control") {
            response.expires = parseCacheControl(line.second.constData());
        } else if (header == "expires") {
            response.expires = util::parseTimePoint(line.second.constData());
        }
    }

    int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    switch(responseCode) {
    case 200: {
        QByteArray bytes = reply->readAll();
        if (bytes.isEmpty()) {
            response.data = std::make_shared<std::string>();
        } else {
            response.data = std::make_shared<std::string>(bytes.data(), bytes.size());
        }
        break;
    }
    case 204:
        response.noContent = true;
        break;
    case 304:
        response.notModified = true;
        break;
    case 404: {
        if (resource.kind == Resource::Kind::Tile) {
            response.noContent = true;
        } else {
            response.error = std::make_unique<Error>(
                Error::Reason::NotFound, "HTTP status code 404");
        }
        break;
    }
    default:
        Response::Error::Reason reason = (responseCode >= 500 && responseCode < 600) ?
            Error::Reason::Server : Error::Reason::Other;

        response.error = std::make_unique<Error>(
            reason, "HTTP status code " + util::toString(responseCode));
    }

    notify(response);
    delete this;
}

void HTTPQtRequest::cancel()
{
    m_context->cancel(this);
    delete this;
}

} // namespace mbgl
