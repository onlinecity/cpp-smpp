/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef CLIENTTEST_H_
#define CLIENTTEST_H_

#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include "smpp/gsmencoding.h"
#include "smpp/smppclient.h"
#include "smpp/smpp.h"
#include "smpp/tlv.h"
#include "smpp/timeformat.h"
#include "connectionsetting.h"

using namespace std;
//using namespace smpp;
using namespace oc::tools;
//using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class ClientTest: public CppUnit::TestFixture {
private:
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::io_service ios;
    boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::shared_ptr<smpp::SmppClient> client;

public:
    ClientTest() :
            endpoint(ip::address_v4::from_string(SMPP_HOST), SMPP_PORT), ios(), socket(
                    boost::shared_ptr<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(ios))), client(
                    boost::shared_ptr<smpp::SmppClient>(new smpp::SmppClient(socket))) {
    }

    void setUp() {
        client->setVerbose(false);
    }

    void tearDown() {
        if (client->isBound())
            client->unbind();
        socket->close();
    }

    /**
     * Test sending a basic SMS
     */
    void testSubmit() {

    }

    /**
     * Test QUERY_SM feature to pull for a message status
     */
    void testQuerySm() {

    }

    /**
     * Test concatenated SMS using either split or payload method.
     */
    void testCsms() {

    }

    /**
     * Test the use of TLVs
     */
    void testTlv() {

    }

    /**
     * Test combining custom TLVs with CSMS TLVs.
     */
    void testTlvExtended() {

    }

    /**
     * Test sending all params to sendSms(). Also sets registered delivery
     */
    void testSubmitExtended() {

    }

    void testReceive() {
        socket->connect(endpoint);
        client->bindReceiver(SMPP_USERNAME, SMPP_PASSWORD);
        SMS sms = client->readSms();
    }

    void testLogging() {

        client->setVerbose(true);

        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        client->unbind();
        socket->close();
    }

CPPUNIT_TEST_SUITE( ClientTest );
    CPPUNIT_TEST(testLogin);
    CPPUNIT_TEST(testSubmit);
    CPPUNIT_TEST(testQuerySm);
    CPPUNIT_TEST(testCsms);
    CPPUNIT_TEST(testTlv);
    CPPUNIT_TEST(testTlvExtended);
    CPPUNIT_TEST(testReceive);
    CPPUNIT_TEST(testLogging);CPPUNIT_TEST_SUITE_END()
    ;

};

#endif /* CLIENTTEST_H_ */
