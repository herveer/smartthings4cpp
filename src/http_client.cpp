#include "smartthings4cpp/http_client.h"

namespace smartthings4cpp {

namespace {
    // A request with no URL never reaches the network; surface it as a normal
    // NetworkError (status 0) instead of handing an empty URL to libcurl, which
    // can crash on some builds. Callers already treat status 0 as NetworkError.
    HttpResponse emptyUrlResponse() {
        HttpResponse result;
        result.status_code = 0;
        result.error_message = "request URL is empty";
        return result;
    }
}

HttpClient::HttpClient() : _timeout(10000), _verify_ssl(true) {}

void HttpClient::setTimeout(std::chrono::milliseconds timeout) {
    _timeout = timeout;
}

void HttpClient::setVerifySsl(bool verify) {
    _verify_ssl = verify;
}

void HttpClient::configureSession(cpr::Session& session) {
    session.SetTimeout(_timeout);
    if (!_verify_ssl) {
        session.SetVerifySsl(cpr::VerifySsl{false});
    }
}

bool HttpClient::hasContentType(const std::map<std::string, std::string>& headers) const {
    for (const auto& [key, value] : headers) {
        if (key == "Content-Type" || key == "content-type") {
            return true;
        }
    }
    return false;
}

HttpResponse HttpClient::convertResponse(const cpr::Response& response) {
    HttpResponse result;
    result.status_code = static_cast<int>(response.status_code);
    result.body = response.text;

    for (const auto& [key, value] : response.header) {
        result.headers[key] = value;
    }

    if (response.error.code != cpr::ErrorCode::OK) {
        result.error_message = response.error.message;
    }

    return result;
}

HttpResponse HttpClient::get(const std::string& url,
                              const std::map<std::string, std::string>& headers) {
    if (url.empty()) {
        return emptyUrlResponse();
    }
    cpr::Session session;
    configureSession(session);
    session.SetUrl(cpr::Url{url});

    if (!headers.empty()) {
        cpr::Header cpr_headers;
        for (const auto& [key, value] : headers) {
            cpr_headers[key] = value;
        }
        session.SetHeader(cpr_headers);
    }

    cpr::Response response = session.Get();
    return convertResponse(response);
}

HttpResponse HttpClient::post(const std::string& url,
                               const std::string& body,
                               const std::map<std::string, std::string>& headers) {
    if (url.empty()) {
        return emptyUrlResponse();
    }
    cpr::Session session;
    configureSession(session);
    session.SetUrl(cpr::Url{url});
    session.SetBody(cpr::Body{body});

    cpr::Header cpr_headers;
    for (const auto& [key, value] : headers) {
        cpr_headers[key] = value;
    }
    if (!hasContentType(headers)) {
        cpr_headers["Content-Type"] = "application/json";
    }
    session.SetHeader(cpr_headers);

    cpr::Response response = session.Post();
    return convertResponse(response);
}

HttpResponse HttpClient::put(const std::string& url,
                              const std::string& body,
                              const std::map<std::string, std::string>& headers) {
    if (url.empty()) {
        return emptyUrlResponse();
    }
    cpr::Session session;
    configureSession(session);
    session.SetUrl(cpr::Url{url});
    session.SetBody(cpr::Body{body});

    cpr::Header cpr_headers;
    for (const auto& [key, value] : headers) {
        cpr_headers[key] = value;
    }
    if (!hasContentType(headers)) {
        cpr_headers["Content-Type"] = "application/json";
    }
    session.SetHeader(cpr_headers);

    cpr::Response response = session.Put();
    return convertResponse(response);
}

HttpResponse HttpClient::del(const std::string& url,
                              const std::map<std::string, std::string>& headers) {
    if (url.empty()) {
        return emptyUrlResponse();
    }
    cpr::Session session;
    configureSession(session);
    session.SetUrl(cpr::Url{url});

    if (!headers.empty()) {
        cpr::Header cpr_headers;
        for (const auto& [key, value] : headers) {
            cpr_headers[key] = value;
        }
        session.SetHeader(cpr_headers);
    }

    cpr::Response response = session.Delete();
    return convertResponse(response);
}

} // namespace smartthings4cpp
