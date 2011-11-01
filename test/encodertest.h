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

		string o1 = smpp::GsmEncoder::getGsm0338(i1);
		string o3 = smpp::GsmEncoder::getUtf8(o1);
		CPPUNIT_ASSERT(i1 == o3);
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("EncoderTest");
		suiteOfTests->addTest(new CppUnit::TestCaller<EncoderTest>("testEncodeDecode", &EncoderTest::testEncodeDecode));
		return suiteOfTests;
	}
};

#endif /* ENCODERTEST_H_ */
