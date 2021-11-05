#include <optional>
#include <string>

namespace talker_helper {

struct Request {
  std::optional<std::string> message;
  std::optional<std::string> login;
  bool is_stop = false;
};

Request MakeRequest(int id, const std::optional<std::string> &session_uuid,
                    const std::optional<std::string> &login);

std::optional<std::string>
ParseFromAnswerSessionUuid(const std::string &response);

} // namespace talker_helper
