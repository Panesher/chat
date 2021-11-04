#include <storages.hpp>

namespace storages {

namespace {
using namespace response;
} // namespace

size_t UsersTable::Size() const {
  return users_.size();
}

Response
UsersTable::InsertUser(const std::string &login, const std::string &password) {
  if (users_.count(login)) {
    return {kStateMismatch};
  }

  return {kOk};
}

Response UsersTable::IsPasswordCorrect(const std::string &login,
                                       const std::string &password) const {
  if (!users_.count(login)) {
    return {kNotFound};
  }
  if (users_.at(login) != password) {
    return {kStateMismatch};
  }
  return {kOk};
}

Response
UsersTable::EraseUser(const std::string &login, const std::string &password) {
  if (!users_.count(login)) {
    return {kNotFound};
  }
  if (users_.at(login) != password) {
    return {kStateMismatch};
  }
  users_.erase(login);
  return {kOk};
}

int MessageTable::InsertMessage(const std::string &message) {
  table_messages_[max_id_++] = message;
  return max_id_ - 1;
}

Response
MessageTable::InsertReciever(int id, const std::string &login_reciever) {
  if (!table_messages_.count(id)) {
    return {kNotFound};
  }
  if (!table_unreaded_messages_.count(login_reciever)) {
    table_unreaded_messages_[login_reciever] = {id};
    return {kOk};
  }
  table_unreaded_messages_[login_reciever].push_back(id);
  return {kOk};
}

std::vector<int> MessageTable::PopReciever(const std::string &login_reciever) {
  if (!table_unreaded_messages_.count(login_reciever)) {
    return {};
  }
  auto res = table_unreaded_messages_[login_reciever];
  table_unreaded_messages_.erase(login_reciever);
  return res;
}

std::string MessageTable::FindMessageById(int id) const {
  if (!table_messages_.count(id)) {
    return "";
  }
  return table_messages_.at(id);
}

int MessageTable::IncreaseMaxId() {
  return (++max_id_) - 1;
}

} // namespace storages