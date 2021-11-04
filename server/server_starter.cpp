#include <iostream>
#include <server.hpp>

int main() {
  boost::shared_ptr<server::Server> serv_ptr(new server::Server(8080));
  serv_ptr->Start();

  return 0;
}
