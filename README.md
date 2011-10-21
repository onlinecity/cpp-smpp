SMPP client implemented in C++
=

This is a simplified SMPP client lib for sending and receiving smses through the SMPP V3.4 protocol.
It only implements a handful of the SMPP features.

Dependencies
----
To build this library you need:
 - [Boost Asio](http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio.html)
 - [Boost.Bimap](http://www.boost.org/doc/libs/1_47_0/libs/bimap/doc/html/index.html)
 - [Boost Regex](http://www.boost.org/doc/libs/1_47_0/libs/regex/doc/html/index.html)
 - [Boost.Smart_ptr](http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/shared_ptr.htm)
 - [Boost.Conversion](http://www.boost.org/doc/libs/1_47_0/libs/conversion/lexical_cast.htm)
 - [Boost.NumericConversion](http://www.boost.org/doc/libs/1_47_0/libs/numeric/conversion/doc/html/index.html)
 - [Boost.Bind](http://www.boost.org/doc/libs/1_47_0/libs/bind/bind.html)
 - [Boost.Date_time](http://www.boost.org/doc/libs/1_47_0/doc/html/date_time.html)
 - [Boost.Thread](http://www.boost.org/doc/libs/1_47_0/doc/html/thread.html)
 - [Boost.Tuple](http://www.boost.org/doc/libs/1_47_0/libs/tuple/doc/tuple_users_guide.html)
 - [Boost.Function](http://www.boost.org/doc/libs/1_47_0/doc/html/function.html)
 
 
We have built this library against boost 1.46

Install
-
```
make
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