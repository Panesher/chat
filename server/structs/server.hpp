#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>
#include <json.hpp>

#include <storages.hpp>

namespace server {

class Server;

class Server : public boost::enable_shared_from_this<Server> {
public:
  class ClientTalker;

  typedef boost::shared_ptr<ClientTalker> ClientPtr;
  typedef boost::system::error_code ErrorCode;
  typedef boost::asio::ip::tcp::acceptor Acceptor;
  typedef boost::asio::ip::tcp::socket Socket;
  using Json = nlohmann::json;

  static const int kBufferSize = 1024;

  Server(size_t port);
  void Start();
  boost::asio::io_service& GetService();
  void AddClient(ClientPtr client);
  void RemoveClient(ClientPtr client);

  response::Response InsertUser(const std::string& login,
                                const std::string& password);
  response::Response IsPasswordCorrect(const std::string& login,
                                       const std::string& password) const;
  response::Response EraseUser(const std::string& login,
                               const std::string& password);

  class ClientTalker : public boost::enable_shared_from_this<ClientTalker>,
                       boost::asio::noncopyable {
  public:
    ClientTalker(boost::shared_ptr<Server> server);

    static ClientPtr NewClient(boost::shared_ptr<Server> server);
    void Start();
    void Stop();
    bool IsStarted() const;

    Socket &GetSocket();

    std::string GetUsername() const;

    response::Response LogIn(const std::string&);

    std::string GetSessionUuid() const;

    int DoWriteToAllOtherClients(const std::string&);
    void DoWriteAllUnreadedMesseges();

    response::Response InsertUser(const std::string& login,
                                  const std::string& password);
    response::Response IsPasswordCorrect(const std::string& login,
                                         const std::string& password) const;
    response::Response EraseUser(const std::string& login,
                                 const std::string& password);

    void DoWrite(std::string message);

  private:
    boost::shared_ptr<Server> my_server_;
    Socket socket_;
    bool started_;
    char read_buffer_[kBufferSize];
    char write_buffer_[kBufferSize];
    std::string session_uuid_;
    std::string login_;

    void OnRead(const ErrorCode &error, size_t bytes_count);
    void DoRead();
    size_t IsReadComplete(const ErrorCode &error, size_t bytes_count);
    
    void ParseMessage(std::string message);

    void OnWrite(const ErrorCode &error, size_t bytes_count);
  };

private:
  boost::asio::io_service service_;
  Acceptor acceptor_;
  std::vector<ClientPtr> clients_;
  storages::UsersTable user_table_;
  storages::MessageTable message_table_;

  void AcceptHandler(ClientPtr client, const ErrorCode &error);
};

} // namespace server
