/*
 * encodertest.h
 *
 *  Created on: Nov 1, 2011
 *      Author: cypres
 */

#ifndef ENCODERTEST_H_
#define ENCODERTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include "gsmencoding.h"
#include <string>

using namespace std;

class EncoderTest: public CppUnit::TestFixture
{

public:

	void setUp()
	{

	}

	void tearDown()
	{

	}

	void testEncodeDecode()
	{

		string i1(
				"Jari i høje @£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉÄÖÑÜ§¿äöñüà^{}\\[~]| Lorem ipsum, Lorem ipsum, Lorem ipsum, Lorem ipsum, Lorem ipsum, Lorem ipsum, ");

		string o1 = oc::tools::GsmEncoder::getGsm0338(i1);
		string o3 = oc::tools::GsmEncoder::getUtf8(o1);
		CPPUNIT_ASSERT_EQUAL(i1, o3);
	}

CPPUNIT_TEST_SUITE(EncoderTest);
		CPPUNIT_TEST(testEncodeDecode);
	CPPUNIT_TEST_SUITE_END();

};

#endif /* ENCODERTEST_H_ */
