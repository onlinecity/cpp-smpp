SMPP client implemented in C++
=

This is a simplified SMPP client lib for sending and receiving smses through the SMPP V3.4 protocol.
It only implements a handful of the SMPP features.

Dependencies
----
To build this library you need and a c++11 compatible compiler:

 - [Asio C++ Library](http://think-async.com)
 - [Boost.Date_time](http://www.boost.org/doc/libs/1_47_0/doc/html/date_time.html)
 - [Google gflags] (https://code.google.com/p/gflags)
 - [Google gtest] (https://code.google.com/p/googletest)
 - [Google glog](https://code.google.com/p/google-glog)

We have built this library against boost 1.46, but it's known to work with boost 1.47 as well.

The following ubuntu packages should suffice for the dependices: [libboost1.46-all-dev](http://packages.ubuntu.com/oneiric/libboost1.46-all-dev) and [libcppunit-dev](http://packages.ubuntu.com/oneiric/libcppunit-dev).

If you dont wan't all the boost libs, try:

 - [libboost-system1.46-dev](http://packages.ubuntu.com/oneiric/libboost-system1.46-dev)
 - [libboost-date-time1.46-dev](http://packages.ubuntu.com/oneiric/libboost-date-time1.46-dev)

Installation
----
``` sh
git clone git@github.com:onlinecity/cpp-smpp.git
cd cpp-smpp
cmake .
make
su
make install
```

There is both an offline unit test (./bin/unittest) and an online unit test (./bin/smppclient_test). If you run ```make test``` you'll run them both via CTest. They can be run individually, which also allows you to view the results from CppUnit. The connection settings for the online test is found in [test/connectionsetting.h](https://github.com/onlinecity/cpp-smpp/blob/master/test/connectionsetting.h).

Sending a SMS:
----

``` c++
//
// Transmitter example
//
// clang++ transmit.cc -I/usr/local/include -lsmpp -lboost_date_time
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
```

Receiving a SMS:
----

``` c++
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

```

F.A.Q.
----

**How do I enable delivery reports?**
You must set the registered delivery flag: ```client.setRegisteredDelivery(smpp::REG_DELIVERY_SMSC_BOTH);```

**Why do I get 'Failed to read reply to command: 0x4', 'Message Length is invalid' or 'Error in optional part' errors?**
Most likely your SMPP provider doesn't support NULL-terminating the message field. The specs aren't clear on this issue, so there is a toggle. Set ```client.setNullTerminateOctetStrings(false);``` and try again.

**Can I test the client library without a SMPP server?**
Many service providers can give you a demo account, but you can also use the [logica opensmpp simulator](http://opensmpp.logica.com/CommonPart/Introduction/Introduction.htm#simulator) (java) or [smsforum client test tool](http://www.smsforum.net/sctt_v1.0.Linux.tar.gz) (linux binary). In addition to a number of real-life SMPP servers this library is tested against these simulators.

**How do I set socket timeouts?**
You cannot modify the connect timeout since it uses the default boost::asio::ip::tcp socket. You can set the socket read/write timeouts by calling ```client.setSocketWriteTimeout(1000)``` and ```client.setSocketReadTimeout(1000)```. All timeouts are in milliseconds.

