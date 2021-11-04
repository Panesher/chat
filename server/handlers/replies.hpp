#pragma once

#include <response.hpp>

#include <server.hpp>

namespace replies {

lib_basics::response::Response
ManageMessage(const nlohmann::json &, server::Server::ClientTalker &);

} // namespace replies
