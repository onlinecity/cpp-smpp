//
// Transmitter example
//
// clang++ transmit.cc -I/usr/local/include -lsmpp -lboost_date_time -lasio
//

#include <iostream>
#include <memory>
#include <string>
#include <asio.hpp>
#include <smpp/gsmencoding.h>
#include <smpp/smpp.h>
#include <smpp/smppclient.h>

int main(int argc, char** argv) {
  asio::io_service io_service;
  std::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(io_service));

  asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::from_string("127.0.0.1"), 2775);
  socket->connect(endpoint);

  smpp::SmppClient smpp_client(socket);
  smpp_client.BindTransmitter("username", "password");

  smpp::SmppAddress sender("CPP-SMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  smpp::SmppAddress receiver("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);

  std::string message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas sollicitudin.";
  auto result = smpp_client.SendSms(sender, receiver, smpp::encoding::GsmEncoder::EncodeGsm0338(message));
  std::cout << "smsc_id:" << result.first << " messages:" << result.second << std::endl;

  smpp_client.Unbind();
  socket->close();
  return 0;
}
