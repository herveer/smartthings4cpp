#include "smartthings4cpp/http_server.h"
#include "smartthings4cpp/json_utils.h"

#include <httplib.h>

#include <atomic>
#include <chrono>
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
					nlohmann::json body;
					try {
						body = json_utils::parse(req.body);
					}
					catch (const std::exception&) {
						res.status = 400;
						res.set_content(R"({"error":"request body is not valid JSON"})", "application/json");
						return;
					}
					res.set_content(onWebhookReceived(std::move(body)).dump(), "application/json");
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
