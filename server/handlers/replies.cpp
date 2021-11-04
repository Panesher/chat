#include <iostream>
#include <optional>

#include <replies.hpp>
#include <request.hpp>

namespace replies {

namespace {

using Json = nlohmann::json;
using namespace response;
using Command = request::Command;

bool IsContain(const Json &data, const std::string& key) {
  if (!data.contains(key)) {
    std::cerr << "Parse error : There is no key \"" << key << "\n";
    return false;
  }
  return true;
}

std::optional<std::string> ParseString(const Json &data, const std::string& key) {
  if (!IsContain(data, key)) {
    return {};
  }
  std::string res;
  try {
    data.at(key).get_to(res);
    return res;
  } catch (nlohmann::detail::type_error ex) {
    std::cerr << "Parse error : " << key << " " << data[key]
              << " cant be parsed as string" << "\n";
    return {};
  }
}

std::optional<int> ParseInt(const Json &data, const std::string& key) {
  if (!IsContain(data, key)) {
    return {};
  }
  int res;
  try {
    data.at(key).get_to(res);
    return res;
  } catch (nlohmann::detail::type_error ex) {
    std::cerr << "Parse error : " << key << " " << data[key]
              << " cant be parsed as integer" << "\n";
    return {};
  }
}

std::optional<request::request> ParseRequest(const Json &data) {
  request::request request;
  auto id = ParseInt(data, "id");
  if (!id) {
    return {};
  }
  request.id = *id;
  auto command = ParseString(data, "command");
  if (!command) {
    return {};
  }
  request.command = request::CommandFromString(*command);
  return request;
}

int SendMessageToAllMembers(
    server::Server::ClientTalker& client, std::string body,
    std::optional<std::string> login = std::nullopt) {
  Json message = {{"message", body}};
  if (login) {
    message.push_back({"sender_login", *login});
  }
  return client.DoWriteToAllOtherClients(message.dump() + "\n");
}

Response OnHello(const request::RequestHello& request,
                 server::Server::ClientTalker& client) {
  std::cout << "Hello" << std::endl;
  client.DoWrite(Json{{"id", request.id},
                      {"command", "HELLO"},
                      {"auth_method", "plain-text"}}.dump() + "\n");
  return {kOk};
}

Response OnAuthorization(const request::RequestAuthorization& request,
                         server::Server::ClientTalker& client) {
  std::cout << "Authorization" << std::endl;
  auto response = client.IsPasswordCorrect(request.login, request.password);
  if (response.status == kOk) {
    response = client.LogIn(request.login);
    if (response.status == kOk) {
      client.DoWrite(Json{{"id",      request.id},
                          {"command", request.command},
                          {"status",  "ok"},
                          {"session", client.GetSessionUuid()}}.dump() + "\n");
      return response;
    }
  }
  client.DoWrite(Json{{"id", request.id},
                      {"command", request.command},
                      {"status", "failed"},
                      {"message", response.AsString()}}.dump() + "\n");

  client.DoWriteAllUnreadedMesseges();
  return response;
}

Response OnRegister(const request::RequestRegister& request,
                    server::Server::ClientTalker& client) {
  std::cout << "Register" << std::endl;
  auto response = client.InsertUser(request.login, request.password);
  if (response.status == kOk) {
    client.DoWrite(Json{{"id", request.id},
                        {"command", request.command},
                        {"status", "ok"},
                        {"message", "Now you can log in"}}.dump() + "\n");
    return response;
  }
  client.DoWrite(Json{{"id", request.id},
                      {"command", request.command},
                      {"status", "failed"},
                      {"message", response.AsString()}}.dump() + "\n");
  return response;
}

Response OnSendMessage(const request::RequestSendMessage& request,
                       server::Server::ClientTalker& client) {
  std::cout << "Send message" << std::endl;
  if (request.session_uuid != client.GetSessionUuid()) {
    Response response = {kStateMismatch};
    client.DoWrite(Json{{"id", request.id},
                        {"command", "message_reply"},
                        {"status", "failed"},
                        {"message", response.AsString()}}.dump() + "\n");
    return response;
  }
  int client_id = SendMessageToAllMembers(client, request.body);
  client.DoWrite(Json{{"id", request.id},
                      {"command", "message_reply"},
                      {"status", "ok"},
                      {"client_id", client_id}}.dump() + "\n");
  return {kOk};
}

Response OnSendMessageFromServer(
    const request::RequestSendMessageFromServer& request,
    server::Server::ClientTalker& client) {
  std::cout << "Send message from server" << std::endl;
  if (request.session_uuid != client.GetSessionUuid()
      || request.sender_login != client.GetUsername()) {
    Response response = {kStateMismatch};
    client.DoWrite(Json{{"id", request.id},
                        {"command", "message_reply"},
                        {"status", "failed"},
                        {"message", response.AsString()}}.dump() + "\n");
    return response;
  }
  int client_id = SendMessageToAllMembers(
      client, request.body, request.sender_login);
  client.DoWrite(Json{{"id", request.id},
                      {"command", "message_reply"},
                      {"status", "ok"},
                      {"client_id", client_id}}.dump() + "\n");
  return {kOk};
}

Response OnLogOut(const request::RequestLogOut& request,
              server::Server::ClientTalker& client) {
  std::cout << "Log out" << std::endl;
  if (request.session_uuid != client.GetSessionUuid()) {
    Response response = {kStateMismatch};
    client.DoWrite(Json{{"id", request.id},
                        {"command", "logout_reply"},
                        {"status", "failed"},
                        {"message", response.AsString()}}.dump() + "\n");
    return response;
  }
  client.Stop();
  client.DoWrite(Json{{"id", request.id},
                      {"command", "logout_reply"},
                      {"status", "ok"}}.dump() + "\n");
  return {kOk};
}

Response OnPing(const request::RequestPing& request,
                server::Server::ClientTalker& client) {
  std::cout << "Ping" << std::endl;
  if (request.session_uuid != client.GetSessionUuid()) {
    Response response = {kStateMismatch};
    client.DoWrite(Json{{"id", request.id},
                        {"command", "ping_reply"},
                        {"status", "failed"},
                        {"message", response.AsString()}}.dump() + "\n");
    return response;
  }
  client.DoWrite(Json{{"id", request.id},
                      {"command", "ping_reply"},
                      {"status", "ok"}}.dump() + "\n");
  return {kOk};
}

} // namespace

Response ManageMessage(
    const Json& data, server::Server::ClientTalker& client) {
  auto request_part = ParseRequest(data);
  if (!request_part) {
    return {kBadRequest};
  }

  switch (request_part->command) {
    case Command::kHello:
      return OnHello({*request_part}, client);
    case Command::kMessage:
      if (auto body = ParseString(data, "body")) {
        if (auto session_id = ParseString(data, "session")) {
          if (IsContain(data, "sender_login")) {
            if (auto sender_login = ParseString(data, "sender_login")) {
              return OnSendMessageFromServer({request_part->id,
                                              request_part->command, *body,
                                              *session_id, *sender_login},
                                             client);
            }
            return {kBadRequest};
          }
          return OnSendMessage({request_part->id, request_part->command,
                                *body, *session_id}, client);
        }
      }
      return {kBadRequest};
    case Command::kPing:
      if (auto session_id = ParseString(data, "session")) {
        return OnPing(
            {request_part->id, request_part->command, *session_id}, client);
      }
      return {kBadRequest};
    case Command::kLogOut:
      if (auto session_id = ParseString(data, "session")) {
        return OnLogOut(
            {request_part->id, request_part->command, *session_id}, client);
      }
      return {kBadRequest};
    case Command::kLogin:
      if (auto login = ParseString(data, "login")) {
        if (auto password = ParseString(data, "password")) {
          return OnAuthorization({request_part->id, request_part->command,
                                  *login, *password}, client);
        }
      }
      return {kBadRequest};
    case Command::kRegister:
      if (auto login = ParseString(data, "login")) {
        if (auto password = ParseString(data, "password")) {
          return OnRegister({request_part->id, request_part->command,
                             *login, *password}, client);
        }
      }
      return {kBadRequest};
  }
  return {kBadRequest};
}

} // namespace replies
