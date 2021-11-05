#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <string>

namespace client {

class Client {
public:
  Client();

  void Connect(boost::asio::ip::tcp::endpoint);

  void Run();

  class Talker {
  public:
    Talker();

    void Connect(const boost::asio::ip::tcp::endpoint&);

    bool TryDoRequest();

    [[nodiscard]] std::string GetLogin() const;

    bool TryRead();

    [[nodiscard]] bool IsStarted() const;

    [[nodiscard]] bool IsConnected() const;

  private:
    std::optional<std::string> ParseFromAnswerSessionUuid();

    void DoWrite(const std::string &msg);

    size_t ReadComplete(const boost::system::error_code &err, size_t bytes);

    bool connected_ = false;
    int id_transaction_ = 0;
    enum { max_msg = 1024 };
    boost::asio::io_service service_;
    boost::asio::ip::tcp::socket socket_;
    size_t already_read_;
    char buff_[max_msg];
    bool started_;
    std::optional<std::string> login_;
    std::optional<std::string> session_uuid_;
    std::optional<std::string> possible_login_;
  };

private:
  Talker talker_;
};
} // namespace client
