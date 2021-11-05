#include <string>

#include <json.hpp>

namespace lib_basics {
namespace parcer_helper {

bool IsContain(const nlohmann::json &data, const std::string &key) {
  if (!data.contains(key)) {
//    std::cerr << "Parse error : There is no key \"" << key << "\n";
    return false;
  }
  return true;
}

std::optional<std::string>
ParseString(const nlohmann::json &data, const std::string &key) {
  if (!IsContain(data, key)) {
    return {};
  }
  std::string res;
  try {
    data.at(key).get_to(res);
    return res;
  } catch (const nlohmann::detail::type_error &ex) {
//    std::cerr << "Parse error : " << key << " " << data[key]
//              << " cant be parsed as string" << "\n";
    return {};
  }
}

std::optional<int>
ParseInt(const nlohmann::json &data, const std::string &key) {
  if (!IsContain(data, key)) {
    return {};
  }
  int res;
  try {
    data.at(key).get_to(res);
    return res;
  } catch (const nlohmann::detail::type_error &ex) {
//    std::cerr << "Parse error : " << key << " " << data[key]
//              << " cant be parsed as integer" << "\n";
    return {};
  }
}

} // namespace parcer_helper
} // namespace lib_basics
