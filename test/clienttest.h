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
using namespace smpp;
using namespace oc::tools;
using namespace boost;
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
     * Test login of either transmitter or receiver
     */
    void testLogin() {

        socket->connect(endpoint);
        CPPUNIT_ASSERT(socket->is_open());
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        CPPUNIT_ASSERT(client->isBound());
        client->unbind();
        socket->close();

        socket->connect(endpoint);
        CPPUNIT_ASSERT(socket->is_open());
        client->bindReceiver(SMPP_USERNAME, SMPP_PASSWORD);
        CPPUNIT_ASSERT(client->isBound());
        client->unbind();
        socket->close();
    }

    /**
     * Test sending a basic SMS
     */
    void testSubmit() {
        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
//		client->setNullTerminateOctetStrings(false);
//		client->setRegisteredDelivery(smpp::REG_DELIVERY_SMSC_BOTH);
        SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
        SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
        string message = "message to send";
        string smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message));
        client->unbind();
        socket->close();
    }

    /**
     * Test QUERY_SM feature to pull for a message status
     */
    void testQuerySm() {
        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
        SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
        string message = "message to send";
        string smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message));
        smpp::QuerySmResult result = client->querySm(smscId, from);
        CPPUNIT_ASSERT_EQUAL(result.get<0>(), smscId);
        client->unbind();
        socket->close();
    }

    /**
     * Test concatenated SMS using either split or payload method.
     */
    void testCsms() {
        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
        SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);

        // Construct 168 char message
        string message;
        message.reserve(170);
        for (int i = 0; i < 14; i++) {
            message += "lorem ipsum ";
        }

        int csmsMethod = client->getCsmsMethod();
        string smscId;

        client->setCsmsMethod(SmppClient::CSMS_PAYLOAD);
        smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message));

        client->setCsmsMethod(SmppClient::CSMS_16BIT_TAGS);
        smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message));

//		client->setCsmsMethod(SmppClient::CSMS_8BIT_UDH);
//		smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message));

        client->setCsmsMethod(csmsMethod);
        client->unbind();
        socket->close();
    }

    /**
     * Test the use of TLVs
     */
    void testTlv() {
        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
        SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
        string message = "message to send";

        list<TLV> taglist;
        taglist.push_back(TLV(smpp::tags::DEST_ADDR_SUBUNIT, static_cast<uint8_t>(0x01)));  // "flash sms" use-case
        taglist.push_back(TLV(smpp::tags::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));
        string smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message), taglist);

        client->unbind();
        socket->close();
    }

    /**
     * Test combining custom TLVs with CSMS TLVs.
     */
    void testTlvExtended() {
        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
        SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);

        string message;
        message.reserve(170);
        for (int i = 0; i < 14; i++) {
            message += "lorem ipsum ";
        }

        list<TLV> taglist;
        taglist.push_back(TLV(smpp::tags::DEST_ADDR_SUBUNIT, static_cast<uint8_t>(0x01)));
        taglist.push_back(TLV(smpp::tags::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));

        int csmsMethod = client->getCsmsMethod();
        client->setCsmsMethod(SmppClient::CSMS_16BIT_TAGS);
        string smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message), taglist);
        client->setCsmsMethod(csmsMethod);
        client->unbind();
        socket->close();
    }

    /**
     * Test sending all params to sendSms().
     * Also sets registered delivery
     */
    void testSubmitExtended() {
        using namespace boost::posix_time;
        using namespace boost::gregorian;
        using namespace boost::local_time;

        socket->connect(endpoint);
        client->bindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
        SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
        SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
        string message = "message to send";

        list<TLV> taglist;
        taglist.push_back(TLV(smpp::tags::DEST_ADDR_SUBUNIT, static_cast<uint8_t>(0x01)));  // "flash sms" use-case
        taglist.push_back(TLV(smpp::tags::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));

        time_zone_ptr gmt(new posix_time_zone("GMT"));
        local_date_time ldt(local_sec_clock::local_time(gmt));
        ldt += time_duration(0, 5, 0);
        string sdt = timeformat::getTimeString(ldt);  // send in five minutes
        string vt = timeformat::getTimeString(time_duration(1, 0, 0));  // valid for one hour

        uint8_t regdlr = client->getRegisteredDelivery();
        client->setRegisteredDelivery(smpp::REG_DELIVERY_SMSC_BOTH);
        string smscId = client->sendSms(from, to, GsmEncoder::getGsm0338(message), taglist, 0x01, sdt, vt,
                                        smpp::DATA_CODING_ISO8859_1);
        client->setRegisteredDelivery(regdlr);

        client->unbind();
        socket->close();
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
