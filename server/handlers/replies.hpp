#pragma once

#include <Response.hpp>

#include <Server.hpp>

namespace replies {

lib_basics::response::Response
ManageMessage(const nlohmann::json &, server::Server::ClientTalker &);

} // namespace replies
