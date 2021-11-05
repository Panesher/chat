#include <Server.hpp>

const int port = 8080;

int main() {
  boost::shared_ptr<server::Server> serv_ptr(new server::Server(port));
  serv_ptr->Start();

  return 0;
}
