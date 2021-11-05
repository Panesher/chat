#include <talker_helper.hpp>

#include <iostream>

#include <json.hpp>
#include <Request.hpp>
#include <Response.hpp>
#include <parcer_helper.hpp>

namespace talker_helper {

namespace {
using namespace lib_basics;
using namespace lib_basics::request;
using Json = nlohmann::json;

const std::string kBadRequestAsString = "bad request";
const std::vector<std::string> kKeysForParsing = {"id", "command", "status",
                                                  "message", "client_id",
                                                  "session"};
const size_t min_login_length = 4;

Request WriteCommandList() {
  std::cout
      << "'HELLO' - command use to hello server. No need to other fields\n"
         "'register' - register into server. Need login and password\n"
         "'login' - log in. Need login and password\n"
         "'logout' - log out. No need to other fields\n"
         "'message' - send message to other users. Need message, optional"
         " sender login\n"
         "'ping' - ping server. No need to other fields\n"
         "'stop' - stop client. No need to other fields\n"
         "'help' - help. No need to other fields" << std::endl;
  return {};
}

Request MakeLoginRequest(const Command &command, int id) {
  std::string login, password;
  int counter_bad_requests = 0;
  while ((login.size() < min_login_length) && ((++counter_bad_requests) < 5)) {
    std::cout << "Write login at least " << min_login_length << " symbols"
              << std::endl;
    std::cin >> login;
  }
  while ((password.size() < min_login_length) &&
         ((++counter_bad_requests) < 5)) {
    std::cout << "Write password at least " << min_login_length << " symbols"
              << std::endl;
    std::cin >> password;
  }
  if ((login.size() < min_login_length) ||
      (password.size() < min_login_length)) {
    std::cout << "To many bad requests" << std::endl;
    return {};
  }
  return {(Json{{"id",       id},
                {"command",  CommandAsString(command)},
                {"login",    login},
                {"password", password}}.dump()) + "\n"};
}

Request MakeLogoutRequest(const Command &command, int id,
                          const std::optional<std::string> &session_uuid) {
  if (!session_uuid) {
    std::cout << "You should login before you log out" << std::endl;
    return {};
  }
  return {(Json{{"id",      id},
                {"command", CommandAsString(command)},
                {"session", *session_uuid}}.dump()) + "\n"};
}

Request
MakeMessageRequest(int id, const std::optional<std::string> &session_uuid,
                   const std::optional<std::string> &login) {
  if (!session_uuid) {
    std::cout << "You should login before you send message" << std::endl;
    return {};
  }
  std::string message;
  char ans;
  std::cout << "Write your message in one line" << std::endl;
  std::getline(std::cin, message);
  std::cout << "Do you want to add your login? (y/n)" << std::endl;
  std::cin >> ans;

  if (ans == 'y') {
    if (!login) {
      std::cout << "There no login sending without login? (y/n)" << std::endl;
      std::cin >> ans;
      if (ans == 'n') {
        return {};
      }
    } else {
      return {(Json{{"id",           id},
                    {"command",      "message"},
                    {"body",         message},
                    {"session",      *session_uuid},
                    {"sender_login", *login}}.dump()) + "\n"};
    }
  }
  return {(Json{{"id",      id},
                {"command", "message"},
                {"body",    message},
                {"session", *session_uuid}}.dump()) + "\n"};
}

Request MakeHelloRequest(int id) {
  return {(Json{{"id",      id},
                {"command", "HELLO"}}.dump()) + "\n"};
}

void ParseMessage(const Json &answer) {
  auto message = parcer_helper::ParseString(answer, "message");
  if (!message) {
    return;
  }
  std::cout << "You got message ";
  if (auto sender_login = parcer_helper::ParseString(answer, "sender_login")) {
    std::cout << " from " << *sender_login;
  }
  std::cout << ": " << *message;
}

std::optional<std::string> MakeStringFromKeys(const Json &answer) {
  std::string out;
  int counter = 0;
  for (const auto &key: kKeysForParsing) {
    if (auto value = parcer_helper::ParseString(answer, key)) {
      out += "  " + key + ": " + *value + "'\n";
    }
    if (++counter > 2 && out.empty()) {
      return {};
    }
  }
  return "Response from server:\n" + out;
}

std::optional<std::string>
ParseCommand(const Json &answer) {
  if (auto out = MakeStringFromKeys(answer)) {
    std::cout << *out;
  }
  if (auto session_uuid = parcer_helper::ParseString(answer, "session")) {
    // status always Ok if session returned
    return session_uuid;
  }
  return {};
}

} // namespace

Request MakeRequest(int id, const std::optional<std::string> &session_uuid,
                    const std::optional<std::string> &login,
                    std::optional<int> &id_login) {
  std::string command_as_string;
  std::cout << "Write command ('help' to get command list)" << std::endl;
  std::cin >> command_as_string;
  auto command = CommandFromString(command_as_string);
  switch (command) {
    case kHelp:
      return WriteCommandList();
    case kRegister:
    case kLogin:
      return MakeLoginRequest(command, id);
    case kMessage:
      return MakeMessageRequest(id, session_uuid, login);
    case kPing:
    case kLogOut:
      return MakeLogoutRequest(command, id, session_uuid);
    case kStop:
      return {std::nullopt, std::nullopt, true};
    case kHello:
      return MakeHelloRequest(id);
    case kNoSuchCommand:
      std::cout << "No such command" << std::endl;
  }
  return {};
}

std::optional<std::string>
ParseFromAnswerSessionUuid(const std::string &response) {
  Json answer;
  try {
    answer = Json::parse(response);
  } catch (const nlohmann::detail::parse_error &ex) {
    std::cerr << "Parse error : " << response << " cant be parsed to json"
              << "\n";
  }
  ParseMessage(answer);
  return ParseCommand(answer);
}

} // namespace talker_helper
