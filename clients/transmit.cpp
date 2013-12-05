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
using namespace oc::tools;
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
    string message = "message to send";
    string smscId = client.sendSms(from, to, GsmEncoder::getGsm0338(message));

    cout << smscId << endl;
    client.unbind();

    return 0;
}
