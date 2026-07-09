#include "smartthings4cpp/http_server.h"

#include <httplib.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <string>
#include <thread>
#include <utility>

namespace smartthings4cpp {

	namespace {

		std::string normalizeRoute(std::string route, const char* fallback) {
			if (route.empty()) {
				return fallback;
			}
			if (route.front() != '/') {
				route.insert(route.begin(), '/');
			}
			return route;
		}

		std::string toLowerAscii(std::string s) {
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		/**
		 * The embedded server behind makeDefaultHttpServer(): cpp-httplib bound
		 * to 127.0.0.1, listening on a background thread. Kept out of any public
		 * header so httplib.h stays an implementation detail.
		 */
		class DefaultHttpServer : public IHttpServer {
		public:
			DefaultHttpServer(int port, std::string oauthRoute, std::string webhookRoute, std::string externalUri)
				: _requestedPort(port),
				_oauthRoute(normalizeRoute(std::move(oauthRoute), "/oauth/callback")),
				_webhookRoute(normalizeRoute(std::move(webhookRoute), "/")),
				_externalUri(externalUri)
			{

				_server.Get(_oauthRoute, [this](const httplib::Request& req, httplib::Response& res) {
					// req.target preserves the raw "<path>?<query>" - exactly what
					// OAuth2Authenticator::completeAuthorization() knows how to parse.
					onOAuthReceived(req.target.empty() ? req.path : req.target);
					res.set_content(
						"<html><body><h3>Authorization received.</h3>"
						"<p>You can close this tab and return to the application.</p>"
						"</body></html>",
						"text/html");
					});

				_server.Post(_webhookRoute, [this](const httplib::Request& req, httplib::Response& res) {
					// Forward the request verbatim: the body is left untouched (its
					// SHA-256 must still match the Digest header) and the headers +
					// request line are carried across so the callback can verify the
					// HTTP-Signature. All body validation now lives in the callback.
					WebhookRequest request;
					request.method = req.method;
					request.path = req.target.empty() ? req.path : req.target;
					request.body = req.body;
					for (const auto& header : req.headers) {
						std::string key = toLowerAscii(header.first);
						auto existing = request.headers.find(key);
						if (existing == request.headers.end()) {
							request.headers.emplace(std::move(key), header.second);
						}
						else {
							existing->second += ", " + header.second; // combine repeats
						}
					}

					WebhookResponse response = onWebhookReceived(request);
					res.status = response.statusCode;
					res.set_content(response.body, "application/json");
					});
			}

			~DefaultHttpServer() override {
				stop();
			}

			Result<void> start() override {
				if (_running) {
					return Result<void>();
				}

				if (_requestedPort == 0) {
					int port = _server.bind_to_any_port("127.0.0.1");
					if (port <= 0) {
						return Result<void>(ErrorCode::NetworkError,
							"Failed to bind the embedded HTTP server to an ephemeral port");
					}
					_actualPort = port;
				}
				else {
					if (!_server.bind_to_port("127.0.0.1", _requestedPort)) {
						return Result<void>(ErrorCode::NetworkError,
							"Failed to bind the embedded HTTP server to port "
							+ std::to_string(_requestedPort) + " (already in use?)");
					}
					_actualPort = _requestedPort;
				}

				_running = true;
				_thread = std::thread([this]() { _server.listen_after_bind(); });

				// Bounded readiness wait so callers can rely on the server
				// accepting connections once start() returns.
				for (int i = 0; i < 500 && !_server.is_running(); ++i) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				if (!_server.is_running()) {
					stop();
					return Result<void>(ErrorCode::NetworkError,
						"The embedded HTTP server did not become ready");
				}
				return Result<void>();
			}

			void stop() override {
				if (!_running) {
					return;
				}
				_server.stop();
				if (_thread.joinable()) {
					_thread.join();
				}
				_running = false;
			}

			std::string fullOAuthCallbackUri() override {
				return _externalUri + _oauthRoute;
			}

		private:
			int _requestedPort;
			int _actualPort = 0;
			std::string _oauthRoute;
			std::string _webhookRoute;
			std::string _externalUri;

			httplib::Server _server;
			std::thread _thread;
			std::atomic<bool> _running{ false };
		};

	} // namespace

	std::unique_ptr<IHttpServer> makeDefaultHttpServer(int port,
		std::string oauthCallbackRoute, std::string webhookCallbackRoute, std::string externalUri) {
		return std::make_unique<DefaultHttpServer>(port,
			std::move(oauthCallbackRoute), std::move(webhookCallbackRoute), externalUri);
	}

} // namespace smartthings4cpp
