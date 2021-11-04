#include <optional>
#include <string>

namespace talker_helper {

struct Request {
  std::optional<std::string> message;
  bool is_stop = false;
};

Request MakeRequest(int id, const std::optional<std::string> &session_uuid,
                    const std::optional<std::string> &login);

void ParseAnswer(const std::string &);

} // namespace talker_helper
