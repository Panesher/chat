#include <string>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>

namespace lib_basics {
namespace request {

enum Command {
  kHello,
  kLogin,
  kMessage,
  kPing,
  kRegister,
  kLogOut,
  kHelp,
  kStop,
  kNoSuchCommand,
};

namespace {

const std::unordered_map<Command, std::string> kCommandMap = {
    {kHello,         "HELLO"},
    {kLogin,         "login"},
    {kMessage,       "message"},
    {kPing,          "ping"},
    {kRegister,      "register"},
    {kLogOut,        "logout"},
    {kStop,          "stop"},
    {kHelp,          "help"},
    {kNoSuchCommand, "no_such_command"}
};

const std::unordered_map<std::string, Command> kCommandMapReversed = {
    {"HELLO",    kHello},
    {"login",    kLogin},
    {"message",  kMessage},
    {"ping",     kPing},
    {"register", kRegister},
    {"stop",     kStop},
    {"help",     kHelp},
    {"logout",   kLogOut}
};

} // namespace

std::string CommandAsString(Command command) {
  try {
    return kCommandMap.at(command);
  } catch (const std::out_of_range &e) {
    return "no_such_command";
  }
}

Command CommandFromString(const std::string &command) {
  try {
    return kCommandMapReversed.at(command);
  } catch (const std::out_of_range &e) {
    return kNoSuchCommand;
  }
}

struct request {
  int id;
  Command command;
};

struct RequestAuthorization : public request {
  std::string login, password;
};

struct RequestSendMessage : public request {
  std::string body;
  std::string session_uuid;
};

struct RequestSendMessageFromServer : public RequestSendMessage {
  std::string sender_login;
};

struct RequestPing : public request {
  std::string session_uuid;
};

typedef RequestPing RequestLogOut;
typedef RequestAuthorization RequestRegister;
typedef request RequestHello;

} // namespace request
} // namespace lib_basics