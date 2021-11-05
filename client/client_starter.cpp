#include <Client.hpp>
#include <boost/asio.hpp>

const boost::asio::ip::tcp::endpoint ep(
    boost::asio::ip::address::from_string("127.0.0.1"), 8080);

int main() {
  client::Client client;
  client.Connect(ep);
  client.Run();
  return 0;
}
