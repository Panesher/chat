#include <Client.hpp>
#include <boost/asio.hpp>
#include <iostream>

const int port = 8080;

int main() {
  std::string ip;
  std::cout << "Write server ip ('127.0.0.1' or 'lh' empty if localhost)"
            << std::endl;
  std::cin >> ip;
  if (ip == "lh") {
    ip = "127.0.0.1";
  }
  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip),
                                    port);
  client::Client client;
  client.Connect(ep);
  client.Run();

  return 0;
}
