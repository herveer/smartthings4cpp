#pragma once

#include "smartthings4cpp/http_server.h"

#include <string>

// Helper for the handful of tests that need a real HTTP server on loopback.
// They drive the library's OWN embedded server (makeDefaultHttpServer) rather
// than standing up a second HTTP stack, so nothing here links httplib or a
// socket library - a second copy of httplib in the test executable clashes with
// the one the library already embeds.
//
// The wrinkle these tests have to work around: fullOAuthCallbackUri() now echoes
// the externalUri it was handed, not the bound listen address, so an ephemeral
// (port 0) server can't be discovered after the fact. Instead we pin an explicit
// free port and make the externalUri point back at it, so the server is reachable
// at exactly the URI it advertises.

namespace testutil {

	// A loopback port that is free right now, found by asking the library's own
	// server to bind it and letting go again. Inherently racy, but fine on
	// loopback: the probe never accepts a connection, so its port frees cleanly.
	inline int freeLoopbackPort() {
		for (int port = 49200; port < 50000; ++port) {
			auto probe = smartthings4cpp::makeDefaultHttpServer(
				port, "/probe", "/probe", "http://localhost");
			if (probe->start().isSuccess()) {
				probe->stop();
				return port;
			}
		}
		return 0; // no free port in range; caller's start()/REQUIRE will surface it
	}

} // namespace testutil
