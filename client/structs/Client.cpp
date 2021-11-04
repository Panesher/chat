#include <Client.hpp>

#include <boost/thread.hpp>
#include <iostream>
#include <talker_helper.hpp>

namespace client {

namespace {
using namespace boost::asio;
} // namespace

Client::Client() : talker_(), is_service_run_(false) {}

void Client::Run() {
  is_service_run_ = true;
  while (true) {
    talker_.TryDoRequest();
    is_service_run_ = talker_.IsStarted();
    if (!is_service_run_) {
      std::cout << "service stopped" << std::endl;
      return;
    }
    talker_.DoRead();
  }
}

bool Client::Talker::IsStarted() const {
  return started_;
}

void Client::Connect(ip::tcp::endpoint ep) {
  talker_.Connect(std::move(ep));
}

Client::Talker::Talker() : socket_(service_), started_(true) {}

void Client::Talker::Connect(ip::tcp::endpoint ep) {
  socket_.connect(ep);
  connected_ = true;
}

bool Client::Talker::TryDoRequest() {
  if (!connected_) {
    std::cout << "Connect to server before you do requests" << std::endl;
  }
  auto request = talker_helper::MakeRequest(id_transaction_++, session_uuid_,
                                            login_);
  if (request.is_stop) {
    std::cout << "stop client" << std::endl;
    started_ = true;
    return false;
  }
  if (request.message || connected_) {
    DoWrite(*request.message);
    return true;
  }
  return false;
}

std::string Client::Talker::GetLogin() const {
  if (!login_) return "";
  return *login_;
}

void Client::Talker::DoRead() {
  if (!connected_) {
    return;
  }
  already_read_ = 0;
  read(socket_, buffer(buff_),
       boost::bind(&Client::Talker::ReadComplete, this, _1, _2));
  ParseAnswer();
}

void Client::Talker::ParseAnswer() {
  std::string message_serv(buff_, already_read_);
  if (message_serv.empty()) {
    return;
  }
  std::cout << " Message got from server : " << message_serv;
  if (login_) {
    std::cout << " for " << *login_;
  }
  std::cout << std::endl;
  talker_helper::ParseAnswer(message_serv);
}

void Client::Talker::DoWrite(const std::string &msg) {
  if (login_) {
    std::cout << *login_ << " ";
  }
  std::cout << "send to server : " << msg << std::endl;
  socket_.write_some(buffer(msg));
}

size_t Client::Talker::ReadComplete(const boost::system::error_code &err,
                                    size_t bytes) {
  if (err) {
    return 0;
  }
  already_read_ = bytes;
  bool found = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
  // we read one-by-one until we get to enter, no buffering
  return found ? 0 : 1;
}

} // namespace client
