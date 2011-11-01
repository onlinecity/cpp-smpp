#ifndef TIMETEST_H_
#define TIMETEST_H_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include "timeformat.h"
#include <string>

using namespace std;
using namespace smpp::timeformat;
using namespace boost::local_time;
using namespace boost::gregorian;
using namespace boost::posix_time;

class TimeTest: public CppUnit::TestFixture
{

public:

	void setUp()
	{

	}

	void tearDown()
	{

	}

	void testAbsolute()
	{
		time_zone_ptr gmt(new posix_time_zone("GMT"));

		DatePair pair1 = parseSmppTimestamp("111019080000002+");
		local_date_time ldt1(ptime(date(2011, Oct, 19), time_duration(8, 30, 0)), gmt);
		CPPUNIT_ASSERT(pair1.first == local_date_time(ptime(date(2011, Oct, 19), time_duration(7, 30, 0)), gmt));
		CPPUNIT_ASSERT(pair1.first.zone()->base_utc_offset() == time_duration(0, 30, 0));
		CPPUNIT_ASSERT(!pair1.second.is_not_a_date_time());

		DatePair pair2 = parseSmppTimestamp("111019080000017+");
		CPPUNIT_ASSERT(pair2.first == local_date_time(ptime(date(2011, Oct, 19), time_duration(3, 45, 0)), gmt));
		CPPUNIT_ASSERT(pair2.first.zone()->base_utc_offset() == time_duration(4, 15, 0));
		CPPUNIT_ASSERT(!pair2.second.is_not_a_date_time());

		DatePair pair3 = parseSmppTimestamp("111019080000004-");
		CPPUNIT_ASSERT(pair3.first == local_date_time(ptime(date(2011, Oct, 19), time_duration(9, 00, 0)), gmt));
		CPPUNIT_ASSERT(pair3.first.zone()->base_utc_offset() == time_duration(-1, 0, 0));
		CPPUNIT_ASSERT(!pair3.second.is_not_a_date_time());
	}

	void testRelative()
	{
		DatePair pair1 = parseSmppTimestamp("000002000000000R");
		CPPUNIT_ASSERT(pair1.second == time_duration(48,0,0));
		CPPUNIT_ASSERT(!pair1.first.is_not_a_date_time());

		DatePair pair2 = parseSmppTimestamp("991210233429000R");
		CPPUNIT_ASSERT(pair2.second == time_duration(876143,34,29));
		CPPUNIT_ASSERT(!pair2.first.is_not_a_date_time());
	}

	void testFormats()
	{
		CPPUNIT_ASSERT_NO_THROW(parseSmppTimestamp("111019103011100+"));
		CPPUNIT_ASSERT_NO_THROW(parseSmppTimestamp("000002000000000R"));
		CPPUNIT_ASSERT_THROW(parseSmppTimestamp("11101910301110+"),smpp::SmppException);
		CPPUNIT_ASSERT_THROW(parseSmppTimestamp("000002000000000r"),smpp::SmppException);
		CPPUNIT_ASSERT_THROW(parseSmppTimestamp("0000020000AA000R"),smpp::SmppException);
		CPPUNIT_ASSERT_THROW(parseSmppTimestamp(""),smpp::SmppException);
	}

	void testDlr()
	{
		ptime pt1 = parseDlrTimestamp("1102031337");
		CPPUNIT_ASSERT(pt1 == ptime(date(2011,Feb,3),time_duration(13,37,0)));

		ptime pt2 = parseDlrTimestamp("110203133755");
		CPPUNIT_ASSERT(pt2 == ptime(date(2011,Feb,3),time_duration(13,37,55)));
	}

	void testFormatAbsolute()
	{
		time_zone_ptr copenhagen(new posix_time_zone("CET+1CEST,M3.5.0,M10.5.0/3")); // From /usr/share/zoneinfo/Europe/Copenhagen
		local_date_time ldt1 (ptime(date(2011, Oct, 19), time_duration(7, 30, 0)), copenhagen);
		CPPUNIT_ASSERT(getTimeString(ldt1) == "111019093000008+");
	}

	void testFormatRelative()
	{
		CPPUNIT_ASSERT(getTimeString(time_duration(48,0,0)) == "000002000000000R");
		CPPUNIT_ASSERT(getTimeString(time_duration(875043,34,29)) == "991025033429000R");

		/*
		 * 876143 would overflow 99 years, but can technically be represented by using more than 11 months as the next field
		 */
		CPPUNIT_ASSERT_THROW(getTimeString(time_duration(876143,34,29)),smpp::SmppException); // 876143 would overflow 99 years
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TimeTest");
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeTest>("testFormats", &TimeTest::testFormats));
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeTest>("testAbsolute", &TimeTest::testAbsolute));
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeTest>("testRelative", &TimeTest::testRelative));
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeTest>("testDlr", &TimeTest::testDlr));
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeTest>("testFormatAbsolute", &TimeTest::testFormatAbsolute));
		suiteOfTests->addTest(new CppUnit::TestCaller<TimeTest>("testFormatRelative", &TimeTest::testFormatRelative));
		return suiteOfTests;
	}
};

#endif /* TIMETEST_H_ */
