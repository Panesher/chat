#pragma once

#include <response.hpp>
#include <server.hpp>

namespace replies {

response::Response
ManageMessage(const nlohmann::json &, server::Server::ClientTalker &);

} // namespace replies
