/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 */

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>
#include "gtest/gtest.h"
#include "smpp/timeformat.h"

using std::string;
using smpp::timeformat::ChronoDatePair;
using smpp::timeformat::ParseSmppTimestamp;
namespace sc = std::chrono;

void print(const std::string &tp_id, const sc::time_point<sc::system_clock> &tp) {
  auto t = sc::system_clock::to_time_t(tp);
  std::cout << tp_id << ":"
  << std::put_time(std::gmtime(&t), "%C %F %T %Z %z")
  << std::endl;
}

sc::time_point<sc::system_clock> MakeTimePoint(int yy, int mon, int mday, int hour, int min, int sec, long gmtoff) {
  struct tm tm;
  tm.tm_year = yy - 1900;
  tm.tm_mon = mon - 1;
  tm.tm_mday = mday;
  tm.tm_hour = hour;
  tm.tm_min = min;
  tm.tm_sec = sec;
  tm.tm_isdst = -1;
  tm.tm_gmtoff = gmtoff * 60;
  return sc::system_clock::from_time_t(std::mktime(&tm));
}

struct tm MakeTm(int yy, int mon, int mday, int hour, int min, int sec, long gmtoff) {
  struct tm tm;
  tm.tm_year = yy - 1900;
  tm.tm_mon = mon - 1;
  tm.tm_mday = mday;
  tm.tm_hour = hour;
  tm.tm_min = min;
  tm.tm_sec = sec;
  tm.tm_isdst = -1;
  tm.tm_gmtoff = gmtoff * 60;
  return tm;
}

TEST(TimeTest, ParseAbsolute) {
  ChronoDatePair pair1 = ParseSmppTimestamp("111019080000002+");
  auto time1 = MakeTimePoint(2011, 10, 19, 8, 0, 0, 2 * 15);
  ASSERT_EQ(pair1.first, time1);

  ChronoDatePair pair2 = ParseSmppTimestamp("111019080000017+");
  auto time2 = MakeTimePoint(2011, 10, 19, 8, 0, 0, 17 * 15);
  ASSERT_EQ(pair2.first, time2);

  ChronoDatePair pair3 = ParseSmppTimestamp("111019080000004-");
  auto time3 = MakeTimePoint(2011, 10, 19, 8, 0, 0, -4 * 15);
  ASSERT_EQ(pair3.first, time3);
}

TEST(TimeTest, ParseRelative) {
  auto pair1 = ParseSmppTimestamp("000002000000000R");
  EXPECT_EQ(pair1.second, sc::hours(48));
  auto pair2 = ParseSmppTimestamp("991210233429000R");
  EXPECT_EQ(pair2.second, sc::hours(876143) + sc::minutes(34) + sc::seconds(29));
}

TEST(TimeTest, formats) {
  EXPECT_NO_THROW(ParseSmppTimestamp("111019103011100+"));
  EXPECT_NO_THROW(ParseSmppTimestamp("000002000000000R"));
  EXPECT_THROW(ParseSmppTimestamp("11101910301110+"), smpp::SmppException);
  EXPECT_THROW(ParseSmppTimestamp("000002000000000r"), smpp::SmppException);
  EXPECT_THROW(ParseSmppTimestamp("0000020000AA000R"), smpp::SmppException);
  EXPECT_THROW(ParseSmppTimestamp(""), smpp::SmppException);
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

TEST(TimeTest, FormatAbsolute) {
  string s = smpp::timeformat::ToSmppTimeString(MakeTm(2011, 10, 19, 9, 30, 0, 60));
  EXPECT_EQ(s, string("111019093000004+"));
}

TEST(TimeTest, FormatRelative) {
  using smpp::timeformat::ToSmppTimeString;
  smpp::timeformat::ToSmppTimeString(sc::hours(48) + sc::minutes(65));
  EXPECT_EQ(ToSmppTimeString(sc::hours(48)), string("000002000000000R"));
  EXPECT_EQ(ToSmppTimeString(sc::hours(875043) + sc::minutes(34) + sc::seconds(29)), string("991025033429000R"));
  EXPECT_THROW(ToSmppTimeString(sc::hours(876143) + sc::minutes(34) + sc::seconds(29)),
               smpp::SmppException);  // 876143 would overflow 99 years
}
