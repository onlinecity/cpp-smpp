/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 */

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>
#include "gtest/gtest.h"
#include "smpp/timeformat.h"

using std::string;
using smpp::timeformat::DatePair;
using smpp::timeformat::parseSmppTimestamp;
using smpp::timeformat::parseDlrTimestamp;
using smpp::timeformat::getTimeString;
using boost::local_time::local_date_time;
using boost::local_time::time_zone_ptr;
using boost::local_time::posix_time_zone;
using boost::gregorian::date;
using boost::posix_time::time_duration;
using boost::posix_time::ptime;

TEST(TimeTest, absolute) {
  time_zone_ptr gmt(new posix_time_zone("GMT"));
  DatePair pair1 = parseSmppTimestamp("111019080000002+");
  local_date_time ldt1(ptime(date(2011, boost::gregorian::Oct, 19), time_duration(8, 30, 0)), gmt);
  ASSERT_EQ(pair1.first, local_date_time(ptime(date(2011, boost::gregorian::Oct, 19), time_duration(7,
                                         30, 0)), gmt));
  ASSERT_EQ(pair1.first.zone()->base_utc_offset(), time_duration(0, 30, 0));
  ASSERT_TRUE(!pair1.second.is_not_a_date_time());
  DatePair pair2 = parseSmppTimestamp("111019080000017+");
  ASSERT_EQ(pair2.first, local_date_time(ptime(date(2011, boost::gregorian::Oct, 19), time_duration(3,
                                         45, 0)), gmt));
  ASSERT_EQ(pair2.first.zone()->base_utc_offset(), time_duration(4, 15, 0));
  ASSERT_TRUE(!pair2.second.is_not_a_date_time());
  DatePair pair3 = parseSmppTimestamp("111019080000004-");
  ASSERT_EQ(pair3.first, local_date_time(ptime(date(2011, boost::gregorian::Oct, 19), time_duration(9,
                                         00, 0)), gmt));
  ASSERT_EQ(pair3.first.zone()->base_utc_offset(), time_duration(-1, 0, 0));
  ASSERT_TRUE(!pair3.second.is_not_a_date_time());
}

TEST(TimeTest, relative) {
  DatePair pair1 = parseSmppTimestamp("000002000000000R");
  ASSERT_EQ(pair1.second, time_duration(48, 0, 0));
  ASSERT_TRUE(!pair1.first.is_not_a_date_time());
  DatePair pair2 = parseSmppTimestamp("991210233429000R");
  ASSERT_EQ(pair2.second, time_duration(876143, 34, 29));
  ASSERT_TRUE(!pair2.first.is_not_a_date_time());
}

TEST(TimeTest, formats) {
  EXPECT_NO_THROW(parseSmppTimestamp("111019103011100+"));
  EXPECT_NO_THROW(parseSmppTimestamp("000002000000000R"));
  EXPECT_THROW(parseSmppTimestamp("11101910301110+"), smpp::SmppException);
  EXPECT_THROW(parseSmppTimestamp("000002000000000r"), smpp::SmppException);
  EXPECT_THROW(parseSmppTimestamp("0000020000AA000R"), smpp::SmppException);
  EXPECT_THROW(parseSmppTimestamp(""), smpp::SmppException);
}

TEST(TimeTest, ParseDlrTimestamp) {
  namespace sc = std::chrono;
  struct tm tm;
  tm.tm_year = 2014 - 1900;
  tm.tm_mon = 1;
  tm.tm_mday = 3;
  tm.tm_hour = 13;
  tm.tm_min = 37;
  tm.tm_sec = 0;
  tm.tm_isdst = -1;

  auto exp_time1 = sc::system_clock::from_time_t(std::mktime(&tm));
  auto time1 = smpp::timeformat::ParseDlrTimestamp("1402031337");
  ASSERT_EQ(time1, exp_time1);

  tm.tm_year = 2009 - 1900;
  tm.tm_mon = 5;
  tm.tm_mday = 5;
  tm.tm_hour = 13;
  tm.tm_min = 37;
  tm.tm_sec = 0;
  tm.tm_isdst = -1;

  auto exp_time2 = sc::system_clock::from_time_t(std::mktime(&tm));
  auto time2 = smpp::timeformat::ParseDlrTimestamp("0906051337");
  ASSERT_EQ(time2, exp_time2);
}

TEST(TimeTest, dlr) {
  ptime pt1 = parseDlrTimestamp("1102031337");
  ASSERT_EQ(pt1, ptime(date(2011, boost::gregorian::Feb, 3), time_duration(13, 37, 0)));
  ptime pt2 = parseDlrTimestamp("110203133755");
  ASSERT_EQ(pt2, ptime(date(2011, boost::gregorian::Feb, 3), time_duration(13, 37, 55)));
}

TEST(TimeTest, formatAbsolute) {
  // From /usr/share/zoneinfo/Europe/Copenhagen
  time_zone_ptr copenhagen(new posix_time_zone("CET+1CEST,M3.5.0,M10.5.0/3"));
  local_date_time ldt1(ptime(date(2011, boost::gregorian::Oct, 19), time_duration(7, 30, 0)),
                       copenhagen);
  ASSERT_EQ(getTimeString(ldt1), string("111019093000008+"));
}

TEST(TimeTest, formatRelative) {
  ASSERT_EQ(getTimeString(time_duration(48, 0, 0)), string("000002000000000R"));
  ASSERT_EQ(getTimeString(time_duration(875043, 34, 29)), string("991025033429000R"));
  /*
   * 876143 would overflow 99 years, but can technically be represented by using more than 11 months as the next field
   */
  EXPECT_THROW(getTimeString(time_duration(876143, 34, 29)),
               smpp::SmppException);  // 876143 would overflow 99 years
}

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
