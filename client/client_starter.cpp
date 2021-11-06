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
  try {
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip),
                                      port);
    client::Client client;
    client.Connect(ep);
    std::cout << "Do you want responses been printed? (y/n)" << std::endl;
    char ans;
    std::cin >> ans;
    if (ans == 'y') {
      client.Run();
    } else {
      client.Run(false);
    }
  } catch (const boost::system::system_error &ex) {
    std::cout << "Ip address can't be parsed" << std::endl;
  }

  return 0;
}
