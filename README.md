SMPP client implemented in C++
=

This is a simplified SMPP client lib for sending and receiving smses through the SMPP V3.4 protocol.
It only implements a handful of the SMPP features.

Dependencies
----
To build this library you need:
 
 - [Boost.Asio](http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio.html)
 - [Boost.Bimap](http://www.boost.org/doc/libs/1_47_0/libs/bimap/doc/html/index.html)
 - [Boost.Bind](http://www.boost.org/doc/libs/1_47_0/libs/bind/bind.html)
 - [Boost.Date_time](http://www.boost.org/doc/libs/1_47_0/doc/html/date_time.html)
 - [Boost.Function](http://www.boost.org/doc/libs/1_47_0/doc/html/function.html)
 - [Boost.Conversion](http://www.boost.org/doc/libs/1_47_0/libs/conversion/lexical_cast.htm)
 - [Boost.NumericConversion](http://www.boost.org/doc/libs/1_47_0/libs/numeric/conversion/doc/html/index.html)
 - [Boost.Regex](http://www.boost.org/doc/libs/1_47_0/libs/regex/doc/html/index.html)
 - [Boost.SmartPointers](http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/smart_ptr.htm)
 - [Boost.Thread](http://www.boost.org/doc/libs/1_47_0/doc/html/thread.html)
 - [Boost.Tuple](http://www.boost.org/doc/libs/1_47_0/libs/tuple/doc/tuple_users_guide.html)
  
  
We have built this library against boost 1.46.

Installation
---
``` sh
make
make install
```

Basic usage
-
Sending a SMS:

``` c++
/*
 * transmit.cpp
 * Compile with (on Ubuntu):
 * g++ transmit.cpp -I/usr/local/include -lsmpp -lboost_system -lboost_regex -lboost_date_time
 */

#include <smpp/smppclient.h>
#include <smpp/gsmencoding.h>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace smpp;
using namespace std;

int main(int argc, char** argv) 
{
	boost::asio::io_service io_service;
	tcp::endpoint endpoint(ip::address_v4::from_string("127.0.0.1"), 2775);
	shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
	socket->connect(endpoint);

	SmppClient client(socket);
	client.setVerbose(true);
	client.bindTransmitter("username", "password");

	SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
	SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
	GsmEncoder encoder;
	string message = "message to send";
	string smscId = client.sendSms(from, to, encoder.getGsm0338(message));
	
	cout << smscId << endl;
	client.unbind();

	return 0;
}
```

Receiving a SMS:

``` cpp
boost::asio::io_service io_service;
tcp::endpoint endpoint(ip::address_v4::from_string("127.0.0.1"), 2775);
tcp::socket* socket = new tcp::socket(io_service);
socket->connect(endpoint);

SmppClient client(socket);

SMS sms = client.readSms();
cout << sms << endl;

client.unbind();
```

