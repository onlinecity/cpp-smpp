//
// Receiver example.
//
// clang++ recever.cc -std=c++11 -lsmpp -lboost_date_time
//

#include <iostream>
#include <memory>
#include <string>
#include <asio.hpp>
#include <smpp/gsmencoding.h>
#include <smpp/smpp.h>
#include <smpp/smppclient.h>

int main(int argc, char** argv) {
  asio::io_service ios;
  std::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(ios));

  asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::from_string("127.0.0.1"), 2775);
  socket->connect(endpoint);

  smpp::SmppClient client(socket);
  client.set_verbose(true);
  client.BindReceiver("username", "password");

  smpp::SMS sms = client.ReadSms();
  std::cout << "source addr:" << sms.source_addr << std::endl;
  std::cout << "dest addr:" << sms.dest_addr << std::endl;
  std::cout << "message:" << sms.short_message << std::endl;
  client.Unbind();

  return 0;
}
