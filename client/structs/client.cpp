#include <client.hpp>

#include <boost/thread.hpp>
#include <iostream>
#include <talker_helper.hpp>
#include <request.hpp>
#include <json.hpp>
#include <parcer_helper.hpp>

namespace client {

namespace {
using Json = nlohmann::json;
using namespace boost::asio;
using namespace lib_basics;
} // namespace

Client::Client() : talker_() {}

void Client::Connect(ip::tcp::endpoint ep) {
  talker_.Connect(std::move(ep));
}

Client::Talker::Talker() : socket_(service_), started_(true) {}

void Client::Talker::Connect(ip::tcp::endpoint ep) {
  socket_.connect(ep);
  connected_ = true;
}

void Client::Talker::DoRequest() {
  if (!connected_) {
    std::cout << "Connect to server before you do requests" << std::endl;
  }
  auto request = talker_helper::MakeRequest(id_transaction_++, session_uuid_,
                                            login_);
  if (request.is_stop) {
    std::cout << "stop client" << std::endl;
//    stop
    return;
  }
  if (request.message) {
    return WriteRequest(*request.message);
  }
}

std::string Client::Talker::GetLogin() const {
  if (!login_) return "";
  return *login_;
}

void Client::Talker::WriteRequest(const std::string &request_json) {
  if (!connected_) {
    return;
  }
  DoWrite(request_json);
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
  std::cout << " Message got from server : " << message_serv;
  if (login_) {
    std::cout << " for " << *login_;
  }
  std::cout << std::endl;

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
