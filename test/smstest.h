#ifndef SMSTEST_H_
#define SMSTEST_H_

#include <list>
#include <string>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "smpp/sms.h"
#include "smpp/smpp.h"
#include "smpp/tlv.h"

using namespace std;

class SmsTest: public CppUnit::TestFixture {
public:
    void setUp() {

    }

    void tearDown() {

    }

    /**
     * Test the construct and copy-construct of SMS
     * The purpose of this test is to reveal a corruption by the use of TLV params (and std::copy without resizing)
     */
    void testCopyCtor() {
        uint8_t testheader[] = { 0x00, 0x00, 0x00, 57 };
        uint8_t testdata[] = {
                0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x34, 0x35,
                0x32, 0x36, 0x31, 0x35, 0x39, 0x39, 0x31, 0x37, 0x00, 0x05, 0x00, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c,
                0x74, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x69, 0x00, 0x04, 0x27, 0x00,
                0x01, 0x02 };

        boost::shared_array<uint8_t> head(new uint8_t[4]);
        std::copy(testheader, testheader + 4, head.get());
        boost::shared_array<uint8_t> data(new uint8_t[53]);
        std::copy(testdata, testdata + 53, data.get());

        smpp::PDU pdu(head, data);
        smpp::SMS sms(pdu);
        smpp::SMS sms2(sms);
        CPPUNIT_ASSERT_EQUAL(sms.is_null, sms2.is_null);
        CPPUNIT_ASSERT(!sms2.is_null);

        // Compare TLVs
        CPPUNIT_ASSERT_EQUAL(sms.tlvs.size(), sms2.tlvs.size());
        list<smpp::TLV>::iterator it;
        list<smpp::TLV>::iterator it2;
        it = sms.tlvs.begin();
        it2 = sms2.tlvs.begin();
        while (it != sms.tlvs.end()) {
            CPPUNIT_ASSERT_EQUAL((*it).getTag(), (*it2).getTag());
            CPPUNIT_ASSERT_EQUAL((*it).getLen(), (*it2).getLen());
            CPPUNIT_ASSERT_EQUAL((*it).getOctets(), (*it2).getOctets());
            it++;
            it2++;
        }
    }

    void testDlr() {
        using namespace boost::posix_time;
        using namespace boost::gregorian;
        uint8_t testheader[] = { 0x00, 0x00, 0x00, 0xe4 };
        uint8_t testdata[] = {
                0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x34, 0x35,
                0x32, 0x36, 0x31, 0x35, 0x39, 0x39, 0x31, 0x37, 0x00, 0x05, 0x00, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c,
                0x74, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x88, 0x69, 0x64, 0x3a, 0x64, 0x63,
                0x30, 0x64, 0x63, 0x38, 0x65, 0x63, 0x36, 0x37, 0x65, 0x31, 0x36, 0x30, 0x38, 0x32, 0x34, 0x38, 0x33,
                0x66, 0x39, 0x65, 0x38, 0x63, 0x64, 0x31, 0x62, 0x31, 0x33, 0x35, 0x64, 0x64, 0x20, 0x73, 0x75, 0x62,
                0x3a, 0x30, 0x30, 0x31, 0x20, 0x64, 0x6c, 0x76, 0x72, 0x64, 0x3a, 0x30, 0x30, 0x31, 0x20, 0x73, 0x75,
                0x62, 0x6d, 0x69, 0x74, 0x20, 0x64, 0x61, 0x74, 0x65, 0x3a, 0x31, 0x31, 0x31, 0x30, 0x32, 0x36, 0x31,
                0x36, 0x34, 0x36, 0x20, 0x64, 0x6f, 0x6e, 0x65, 0x20, 0x64, 0x61, 0x74, 0x65, 0x3a, 0x31, 0x31, 0x31,
                0x30, 0x32, 0x36, 0x31, 0x36, 0x34, 0x37, 0x20, 0x73, 0x74, 0x61, 0x74, 0x3a, 0x44, 0x45, 0x4c, 0x49,
                0x56, 0x52, 0x44, 0x20, 0x65, 0x72, 0x72, 0x3a, 0x30, 0x30, 0x30, 0x20, 0x74, 0x65, 0x78, 0x74, 0x3a,
                0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x40, 0x04, 0x27, 0x00, 0x01, 0x02,
                0x00, 0x1e, 0x00, 0x21, 0x64, 0x63, 0x30, 0x64, 0x63, 0x38, 0x65, 0x63, 0x36, 0x37, 0x65, 0x31, 0x36,
                0x30, 0x38, 0x32, 0x34, 0x38, 0x33, 0x66, 0x39, 0x65, 0x38, 0x63, 0x64, 0x31, 0x62, 0x31, 0x33, 0x35,
                0x64, 0x64, 0x00 };

        boost::shared_array<uint8_t> head(new uint8_t[4]);
        std::copy(testheader, testheader + 4, head.get());
        boost::shared_array<uint8_t> data(new uint8_t[0xe0]);
        std::copy(testdata, testdata + 0xe0, data.get());

        smpp::PDU pdu(head, data);
        smpp::SMS sms(pdu);
        smpp::DeliveryReport dlr(sms);

        // Assertions for SMS
        CPPUNIT_ASSERT_EQUAL(sms.source_addr, string("4526159917"));
        CPPUNIT_ASSERT_EQUAL(sms.source_addr_ton, 1);
        CPPUNIT_ASSERT_EQUAL(sms.source_addr_npi, 1);
        CPPUNIT_ASSERT_EQUAL(sms.dest_addr, string("default"));
        CPPUNIT_ASSERT_EQUAL(sms.dest_addr_ton, 5);
        CPPUNIT_ASSERT_EQUAL(sms.dest_addr_npi, 0);
        CPPUNIT_ASSERT_EQUAL(sms.esm_class, smpp::ESM_DELIVER_SMSC_RECEIPT);
        CPPUNIT_ASSERT(!sms.is_null);
        CPPUNIT_ASSERT_EQUAL(sms.data_coding, smpp::DATA_CODING_ISO8859_1);

        // Assertions for TLV fields
        CPPUNIT_ASSERT_EQUAL(static_cast<int>(sms.tlvs.size()), 2);
        list<smpp::TLV>::iterator it;
        it = sms.tlvs.begin();
        CPPUNIT_ASSERT_EQUAL(it->getTag(), smpp::tags::MESSAGE_STATE);
        CPPUNIT_ASSERT_EQUAL(it->getOctets()[0], smpp::STATE_DELIVERED);
        it++;
        CPPUNIT_ASSERT_EQUAL(it->getTag(), smpp::tags::RECEIPTED_MESSAGE_ID);
        CPPUNIT_ASSERT_EQUAL(string(reinterpret_cast<char*>(it->getOctets().get())),
                             string("dc0dc8ec67e16082483f9e8cd1b135dd"));

        // Assertions for DLR part of SMS
        CPPUNIT_ASSERT_EQUAL(dlr.id, string("dc0dc8ec67e16082483f9e8cd1b135dd"));
        CPPUNIT_ASSERT_EQUAL(dlr.sub, uint32_t(1));
        CPPUNIT_ASSERT_EQUAL(dlr.dlvrd, uint32_t(1));
        CPPUNIT_ASSERT_EQUAL(dlr.submitDate, ptime(date(2011, Oct, 26), time_duration(16, 46, 0)));
        CPPUNIT_ASSERT_EQUAL(dlr.doneDate, ptime(date(2011, Oct, 26), time_duration(16, 47, 0)));
        CPPUNIT_ASSERT_EQUAL(dlr.stat, string("DELIVRD"));
        CPPUNIT_ASSERT_EQUAL(dlr.err, string("000"));
    }

CPPUNIT_TEST_SUITE(SmsTest);
    CPPUNIT_TEST(testDlr);
    CPPUNIT_TEST(testCopyCtor);CPPUNIT_TEST_SUITE_END()
    ;
};

#endif /* SMSTEST_H_ */
