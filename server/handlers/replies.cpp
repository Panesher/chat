#include <iostream>
#include <optional>

#include <replies.hpp>
#include <Request.hpp>
#include <parcer_helper.hpp>

namespace replies {

namespace {
using namespace lib_basics;
using namespace response;
using namespace parcer_helper;
using Command = request::Command;
using Json = nlohmann::json;

std::optional<request::Request> ParseRequest(const Json &data) {
  request::Request request;
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

int
SendMessageToAllMembers(server::Server::ClientTalker &client, std::string body,
                        std::optional<std::string> login = std::nullopt) {
  Json message = {{"message", body}};
  if (login) {
    message.push_back({"sender_login", *login});
  }
  return client.DoWriteToAllOtherClients(message.dump() + "\n");
}

Response OnHello(const request::RequestHello &request,
                 server::Server::ClientTalker &client) {
  std::cout << "Hello" << std::endl;
  client.DoWrite(Json{{"id",          request.id},
                      {"command",     "HELLO"},
                      {"auth_method", "plain-text"}}.dump() + "\n");
  return {kOk};
}

Response OnAuthorization(const request::RequestAuthorization &request,
                         server::Server::ClientTalker &client) {
  std::cout << "Authorization" << std::endl;
  auto response = client.LogIn(request.login, request.password);
  if (response.status == kOk) {
    client.DoWrite(Json{{"id",      request.id},
                        {"command", CommandAsString(request.command)},
                        {"status",  "ok"},
                        {"session", client.GetSessionUuid()}}.dump() + "\n");
    client.DoWriteAllUnreadedMesseges();
    return response;
  }
  client.DoWrite(Json{{"id",      request.id},
                      {"command", CommandAsString(request.command)},
                      {"status",  "failed"},
                      {"message", response.AsString()}}.dump() + "\n");
  return response;
}

Response OnRegister(const request::RequestRegister &request,
                    server::Server::ClientTalker &client) {
  std::cout << "Register" << std::endl;
  auto response = client.InsertUser(request.login, request.password);
  if (response.status == kOk) {
    client.DoWrite(Json{{"id",      request.id},
                        {"command", CommandAsString(request.command)},
                        {"status",  "ok"},
                        {"message", "Now you can log in"}}.dump() + "\n");
    return response;
  }
  client.DoWrite(Json{{"id",      request.id},
                      {"command", CommandAsString(request.command)},
                      {"status",  "failed"},
                      {"message", response.AsString()}}.dump() + "\n");
  return response;
}

Response OnSendMessage(const request::RequestSendMessage &request,
                       server::Server::ClientTalker &client) {
  std::cout << "Send message" << std::endl;
  Response response = {kOk};
  if (request.session_uuid != client.GetSessionUuid()) {
    response = {kStateMismatch};
  }
  if (client.isAuthorizated()) {
    response = {kUnauthorized};
  }
  if (response.status == kOk) {
    int client_id = SendMessageToAllMembers(client, request.body);
    client.DoWrite(Json{{"id",        request.id},
                        {"command",   "message_reply"},
                        {"status",    "ok"},
                        {"client_id", client_id}}.dump() + "\n");
    return response;
  }
  client.DoWrite(Json{{"id",      request.id},
                      {"command", "message_reply"},
                      {"status",  "failed"},
                      {"message", response.AsString()}}.dump() + "\n");
  return response;
}

Response
OnSendMessageFromServer(const request::RequestSendMessageFromServer &request,
                        server::Server::ClientTalker &client) {
  std::cout << "Send message with login" << std::endl;
  Response response = {kOk};
  if (request.session_uuid != client.GetSessionUuid() ||
      request.sender_login != client.GetUsername()) {
    response = {kStateMismatch};
  }
  if (client.isAuthorizated()) {
    response = {kUnauthorized};
  }
  if (response.status == kOk) {
    int client_id = SendMessageToAllMembers(client, request.body,
                                            request.sender_login);
    client.DoWrite(Json{{"id",        request.id},
                        {"command",   "message_reply"},
                        {"status",    "ok"},
                        {"client_id", client_id}}.dump() + "\n");
    return response;
  }
  client.DoWrite(Json{{"id",      request.id},
                      {"command", "message_reply"},
                      {"status",  "failed"},
                      {"message", response.AsString()}}.dump() + "\n");
  return response;
}

Response OnLogOut(const request::RequestLogOut &request,
                  server::Server::ClientTalker &client) {
  std::cout << "Log out" << std::endl;
  if (request.session_uuid != client.GetSessionUuid()) {
    Response response = {kStateMismatch};
    client.DoWrite(Json{{"id",      request.id},
                        {"command", "logout_reply"},
                        {"status",  "failed"},
                        {"message", response.AsString()}}.dump() + "\n");
    return response;
  }
  client.Stop();
  client.DoWrite(Json{{"id",      request.id},
                      {"command", "logout_reply"},
                      {"status",  "ok"}}.dump() + "\n");
  return {kOk};
}

Response OnPing(const request::RequestPing &request,
                server::Server::ClientTalker &client) {
  std::cout << "Ping" << std::endl;
  if (request.session_uuid != client.GetSessionUuid()) {
    Response response = {kStateMismatch};
    client.DoWrite(Json{{"id",      request.id},
                        {"command", "ping_reply"},
                        {"status",  "failed"},
                        {"message", response.AsString()}}.dump() + "\n");
    return response;
  }
  client.DoWrite(Json{{"id",      request.id},
                      {"command", "ping_reply"},
                      {"status",  "ok"}}.dump() + "\n");
  return {kOk};
}

Response WriteBadRequest(server::Server::ClientTalker &client,
                         const std::optional<int> &id = std::nullopt,
                         const std::optional<Command> &command = std::nullopt) {
  Response response = {kBadRequest};
  Json resp = Json{{"status", response.AsString()}};
  if (id) {
    resp.push_back({"id", *id});
  }
  if (command) {
    resp.push_back({"command", request::CommandAsString(*command)});
  }
  client.DoWrite(resp.dump() + "\n");
  return response;
}

} // namespace

Response ManageMessage(const Json &data, server::Server::ClientTalker &client) {
  auto request_part = ParseRequest(data);
  if (!request_part) {
    return WriteBadRequest(client);
  }

  switch (request_part->command) {
    case Command::kHello:
      return OnHello({*request_part}, client);
    case Command::kMessage:
      if (auto body = ParseString(data, "body")) {
        if (auto session_id = ParseString(data, "session")) {
          if (IsContain(data, "sender_login")) {
            if (auto sender_login = ParseString(data, "sender_login")) {
              return OnSendMessageFromServer(
                  {request_part->id, request_part->command, *body, *session_id,
                   *sender_login}, client);
            }
            return WriteBadRequest(client, request_part->id, request_part->command);
          }
          return OnSendMessage(
              {request_part->id, request_part->command, *body, *session_id},
              client);
        }
      }
      return WriteBadRequest(client, request_part->id, request_part->command);
    case Command::kPing:
      if (auto session_id = ParseString(data, "session")) {
        return OnPing({request_part->id, request_part->command, *session_id},
                      client);
      }
      return WriteBadRequest(client, request_part->id, request_part->command);
    case Command::kLogOut:
      if (auto session_id = ParseString(data, "session")) {
        return OnLogOut({request_part->id, request_part->command, *session_id},
                        client);
      }
      return WriteBadRequest(client, request_part->id, request_part->command);
    case Command::kLogin:
      if (auto login = ParseString(data, "login")) {
        if (auto password = ParseString(data, "password")) {
          return OnAuthorization(
              {request_part->id, request_part->command, *login, *password},
              client);
        }
      }
      return WriteBadRequest(client, request_part->id, request_part->command);
    case Command::kRegister:
      if (auto login = ParseString(data, "login")) {
        if (auto password = ParseString(data, "password")) {
          return OnRegister(
              {request_part->id, request_part->command, *login, *password},
              client);
        }
      }
      return WriteBadRequest(client, request_part->id, request_part->command);
  }
  return WriteBadRequest(client, request_part->id, request_part->command);
}

} // namespace replies
