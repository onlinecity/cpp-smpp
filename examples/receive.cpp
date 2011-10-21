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

