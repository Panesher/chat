#include <boost/thread/mutex.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

#include <Response.hpp>

namespace server {
class Server;
} // namespace server

/// Should mark that this part better write with normal DB and SQL, but I'm lazy :)
namespace storages {

class UsersTable {
private:
  std::unordered_map<std::string, std::string> users_;
  boost::mutex mutex_;

  friend server::Server;

public:
  lib_basics::response::Response
  InsertUser(const std::string &login, const std::string &password);

  lib_basics::response::Response
  IsPasswordCorrect(const std::string &login, const std::string &password);

  lib_basics::response::Response
  EraseUser(const std::string &login, const std::string &password);

  size_t Size() const;
};

class MessageTable {
private:
  std::unordered_map<int, std::string> table_messages_;
  std::unordered_map<std::string, std::vector<int>> table_unreaded_messages_;
  boost::mutex mutex_;
  int max_id_ = 0;

public:
  int InsertMessage(const std::string &message);

  lib_basics::response::Response
  InsertReciever(int id, const std::string &login_reciever);

  std::vector<int> PopReciever(const std::string &login_reciever);

  std::string FindMessageById(int id);

  int IncreaseMaxId();
};

} // namespace storages