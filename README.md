SMPP client implemented in C++
=

This is a simplified SMPP client lib for sending and receiving smses through the SMPP V3.4 protocol.
It only implements a handful of the SMPP features.

Dependencies
-
To build this library you need:
 * [Boost Asio](http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio.html)
 * [Boost Regex](http://www.boost.org/doc/libs/1_47_0/libs/regex/doc/html/index.html)

Install
-
```
make install
```

Basic usage
-
To send a SMS:

```
boost::asio::io_service io_service;
tcp::endpoint endpoint(ip::address_v4::from_string("127.0.0.1"), 2775);
tcp::socket* socket = new tcp::socket(io_service);
socket->connect(endpoint);

SmppClient client(socket);

client.bindTransmitter("username", "password");
SmppAddress from("cppSmpp", smpp::TON_ALPHANUMERIC, smpp::DATA_CODING_DEFAULT);
SmppAddress to("45123456789", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
string message = "message to send";

GsmEncoder encoder;

client.sendSms(from, to, encoder.getGsm0338(lorem));
client.unbind();
```

To receive SMS:

```
boost::asio::io_service io_service;
tcp::endpoint endpoint(ip::address_v4::from_string("127.0.0.1"), 2775);
tcp::socket* socket = new tcp::socket(io_service);
socket->connect(endpoint);

SmppClient client(socket);

SMS sms = client.readSms();
cout << sms << endl;

client.unbind();
```