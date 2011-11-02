#ifndef PDUTEST_H_
#define PDUTEST_H_

#include <stdint.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/shared_ptr.hpp>
#include "pdu.h"

using namespace std;

class PduTest: public CppUnit::TestFixture
{

public:

	void setUp()
	{

	}

	void tearDown()
	{

	}

	void testReadWrite()
	{
		uint32_t commandId = 1;
		uint32_t commandStatus = 2;
		uint32_t sequence = 3;
		smpp::PDU pdu(commandId, commandStatus, sequence);

		string str("test");
		uint32_t i32 = 0xdeadbeef;
		uint16_t i16 = 0x1337;
		uint8_t i8 = 0x80;

		pdu << str;
		pdu << i32;
		pdu << i16;
		pdu << i8;

		pdu.resetMarker();

		string s;
		uint32_t o32;
		uint16_t o16;
		uint8_t o8;

		pdu >> s;
		CPPUNIT_ASSERT(str == s);
		pdu >> o32;
		CPPUNIT_ASSERT(o32 == i32);
		pdu >> o16;
		CPPUNIT_ASSERT(o16 == i16);
		pdu >> o8;
		CPPUNIT_ASSERT(o8 == i8);
	}

	void testBinary()
	{

		uint8_t testheader[] = { 0x00, 0x00, 0x00, 0x1c };
		uint8_t testdata[] = {
				0x00,0x00,0x00,0x01,
				0x00,0x00,0x00,0x02,
				0x00,0x00,0x00,0x03,
				0x74,0x65,0x73,0x74,0x00,
				0xde,0xad,0xbe,0xef,
				0x13,0x37,
				0x80
		};

		boost::shared_array<uint8_t> head(new uint8_t[4]);
		std::copy(testheader, testheader + 4, head.get());
		boost::shared_array<uint8_t> data(new uint8_t[0x1c-4]);
		std::copy(testdata, testdata + 0x1c-4, data.get());

		smpp::PDU pdu(head, data);

		string s;
		uint32_t o32;
		uint16_t o16;
		uint8_t o8;

		pdu >> s;
		CPPUNIT_ASSERT(s == "test");
		pdu >> o32;
		CPPUNIT_ASSERT(o32 == 0xdeadbeef);
		pdu >> o16;
		CPPUNIT_ASSERT(o16 == 0x1337);
		pdu >> o8;
		CPPUNIT_ASSERT(o8 == 0x80);
	}

	CPPUNIT_TEST_SUITE(PduTest);
	CPPUNIT_TEST(testReadWrite);
	CPPUNIT_TEST(testBinary);
	CPPUNIT_TEST_SUITE_END();
};

#endif /* PDUTEST_H_ */
