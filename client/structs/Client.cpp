#include <Client.hpp>

#include <boost/thread.hpp>
#include <iostream>
#include <talker_helper.hpp>

namespace client {

namespace {
using namespace boost::asio;
const std::string kEndOfFile = "read: End of file";
} // namespace

Client::Client() : talker_() {}

void Client::Run() try {
  if (!talker_.IsConnected()) {
    std::cout << "Connect to server before you do requests" << std::endl;
    return;
  }
  while (true) {
    talker_.TryDoRequest();
    if (!talker_.IsConnected()) {
      std::cout << "Service stopped." << std::endl;
      return;
    }
    talker_.TryRead();
    if (!talker_.IsConnected()) {
      std::cout << "Service stopped." << std::endl;
      return;
    }
  }
} catch (const boost::system::system_error &ex) {
  if (ex.what() == kEndOfFile) {
    std::cout
        << "Connection dropped. Last request hadn't sent. Service stopped."
        << std::endl;
  }
}

void Client::Talker::Disconnect() {
  connected_ = false;
  socket_.close();
  std::cout << "Connection has closed." << std::endl;
}

bool Client::Talker::IsConnected() const {
  return connected_;
}

void Client::Connect(ip::tcp::endpoint ep) {
  talker_.Connect(std::move(ep));
}

Client::Talker::Talker() : socket_(service_), already_read_(0), buff_() {}

void Client::Talker::Connect(const ip::tcp::endpoint &ep) {
  try {
    socket_.connect(ep);
  } catch (const boost::exception &e) {
    std::cout << "Can't connect to server" << std::endl;
    return;
  }
  connected_ = true;
}

void Client::Talker::TryDoRequest() {
  if (!connected_) {
    std::cout << "Connect to server before you do requests" << std::endl;
  }
  auto request = talker_helper::MakeRequest(++id_transaction_, session_uuid_,
                                            login_);
  if (request.is_stop) {
    return Disconnect();
  }
  if (request.login) {
    possible_login_ = request.login;
  }
  if (request.message && connected_) {
    DoWrite(*request.message);
    return;
  }
  return TryDoRequest(); // tail recursion
}

std::string Client::Talker::GetLogin() const {
  if (!login_) {
    return "";
  }
  return *login_;
}

void Client::Talker::TryRead() {
  if (!connected_) {
    return;
  }
  already_read_ = 0;
  read(socket_, buffer(buff_),
       boost::bind(&Client::Talker::IsReadComplete, this, _1, _2));

  if (ParseResponse()) {
    return TryRead(); // tail recursion
  }
}

bool Client::Talker::ParseResponse() {
  if ((already_read_ < 1) || (buff_[0] != '{')) {
    return true;
  }
  std::string message_serv(buff_, already_read_);
  if (message_serv.empty()) {
    return false;
  }
  auto response = talker_helper::ParseResponse(message_serv);
  if (response.session_uuid) {
    session_uuid_ = response.session_uuid;
    login_ = possible_login_;
  }
  if (!response.is_connected) {
    Disconnect();
    return false;
  }
  if (response.is_message) {
    return true;
  }
  return false;
}

void Client::Talker::DoWrite(const std::string &msg) {
  if (msg.empty()) {
    return;
  }
  socket_.write_some(buffer(msg));
  std::cout << "Request sent" << std::endl;
}

size_t Client::Talker::IsReadComplete(const boost::system::error_code &err,
                                      size_t bytes) {
  if (err) {
    return 0;
  }
  already_read_ = bytes;

  if (already_read_ && (buff_[0] != '{')) {
    return 0;
  }

  size_t left = std::count(buff_, buff_ + bytes, '{');
  size_t right = std::count(buff_, buff_ + bytes, '}');
  bool found = left == right && left != 0;

  return found ? 0 : 1;
}

} // namespace client
