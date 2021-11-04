//#include <client.hpp>
#include <iostream>
#include <json.hpp>

//using namespace boost::asio;
//
//ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 8080);
//void RunClient(const std::string & client_name) {
//   client::Client client(client_name);
//  try {
//    client.Connect(ep);
//    client.Loop();
//  } catch(boost::system::system_error & err) {
//    std::cout << "client terminated " << client.username()
//              << ": " << err.what() << std::endl;
//  }
//}

using Json = nlohmann::json;

int main() {
//  std::getline(std::cin, name);
  Json json1({{"field1", "value1"},
              {"field2", "value2"}});
  std::string json_as_string = json1.dump() + "\n" + Json{{"field3", "value3"},
                                                          {"field4", "value4"}}.dump() +
                               "\n";
  std::cout << json1 << std::endl;
  std::cout << json_as_string;
  Json json2 = Json::parse(json_as_string);
  std::cout << json2;
//  RunClient(std::move(name));
}
