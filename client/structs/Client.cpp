#include <Client.hpp>

#include <boost/thread.hpp>
#include <iostream>
#include <talker_helper.hpp>

namespace client {

namespace {
using namespace boost::asio;
} // namespace

Client::Client() : talker_() {}

void Client::Run() {
  while (true) {
    talker_.TryDoRequest();
    if (!talker_.IsConnected() || !talker_.IsStarted()) {
      std::cout << "Service stopped" << std::endl;
      return;
    }
    talker_.TryRead();
    if (!talker_.IsConnected() || !talker_.IsStarted()) {
      std::cout << "Service stopped or disconnected" << std::endl;
      return;
    }
  }
}

bool Client::Talker::IsConnected() const {
  return connected_;
}

bool Client::Talker::IsStarted() const {
  return started_;
}

void Client::Connect(ip::tcp::endpoint ep) {
  talker_.Connect(std::move(ep));
}

Client::Talker::Talker() : socket_(service_), started_(true), already_read_(0), buff_() {}

void Client::Talker::Connect(const ip::tcp::endpoint& ep) {
  try {
    socket_.connect(ep);
  } catch (const boost::exception &e) {
    std::cout << "Can't connect to server" << std::endl;
    return;
  }
  connected_ = true;
}

bool Client::Talker::TryDoRequest() {
  if (!connected_) {
    std::cout << "Connect to server before you do requests" << std::endl;
  }
  auto request = talker_helper::MakeRequest(++id_transaction_, session_uuid_,
                                            login_);
  if (request.is_stop) {
    connected_ = false;
    return false;
  }
  if (request.login) {
    possible_login_ = request.login;
  }
  if (request.message || connected_) {
    DoWrite(*request.message);
    return true;
  }
  return false;
}

std::string Client::Talker::GetLogin() const {
  if (!login_) {
    return "";
  }
  return *login_;
}

bool Client::Talker::TryRead() {
  if (!connected_) {
    return false;
  }
  already_read_ = 0;
  read(socket_, buffer(buff_),
       boost::bind(&Client::Talker::ReadComplete, this, _1, _2));
  if (auto session = ParseFromAnswerSessionUuid()) {
    session_uuid_ = session;
    login_ = possible_login_;
  }
  return true;
}

std::optional<std::string> Client::Talker::ParseFromAnswerSessionUuid() {
  if ((already_read_ < 2) && (buff_[0] == 0)) {
    TryRead();
    return {};
  }
  std::string message_serv(buff_, already_read_);
  if (message_serv.empty()) {
    return {};
  }
//  std::cout << "Message got from server : " << message_serv;
//  if (login_) {
//    std::cout << " for " << *login_;
//  }
//  std::cout << std::endl;
  auto response = talker_helper::ParseResponse(message_serv);
  connected_ = response.is_connected;
  if (response.is_message) {
    TryRead();
    return std::nullopt;
  }
  return response.session_uuid;
}

void Client::Talker::DoWrite(const std::string &msg) {
  if (msg.empty()) {
    return;
  }
//  if (login_) {
//    std::cout << *login_ << " ";
//  }
//  std::cout << "send to server : " << msg << std::endl;
  socket_.write_some(buffer(msg));
}

size_t Client::Talker::ReadComplete(const boost::system::error_code &err,
                                    size_t bytes) {
  if (err) {
    return 0;
  }
  already_read_ = bytes;
  bool found = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
  if ((already_read_ == 1) && (buff_[0] == 0)) {
    return 0;
  }
  // we read one-by-one until we get to enter, no buffering
  return found ? 0 : 1;
}

} // namespace client
