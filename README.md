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
 - [Boost.System](http://www.boost.org/doc/libs/1_47_0/libs/system/doc/index.html)
  
We have built this library against boost 1.46.

Installation
----
``` sh
git clone git@github.com:onlinecity/cpp-smpp.git 
cd cpp-smpp/src
make
su 
make install
```

Sending a SMS:
----

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
----

``` c++
/*
 * receive.cpp
 * Compile with (on ubuntu):
 * g++ receive.cpp -I/usr/local/include -lsmpp -lboost_system -lboost_regex -lboost_date_time
 */

#include <smpp/smppclient.h>
#include <smpp/sms.h>
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
	client.bindReceiver("username", "password");

	SMS sms = client.readSms();
	cout << "source addr: " << sms.source_addr << endl;
	cout << "dest addr: " << sms.dest_addr << endl;
	cout << "SM: " << sms.short_message << endl;

	if ((sms.esm_class & smpp::ESM_DELIVER_SMSC_RECEIPT) != 0) {
		DeliveryReport dlr(sms);
		cout << "id: " << dlr.id << endl;
		cout << "err: " << dlr.err << endl;
		cout << "stat: " << dlr.stat << endl;
		cout << "done date: " << dlr.doneDate << endl;
		cout << "submit date: " << dlr.submitDate << endl;
	}

	client.unbind();

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

