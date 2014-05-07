// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#include "smpp/timeformat.h"
#include <complex>
#include <string>

using std::string;
using std::stoi;

using std::regex;
using std::smatch;

namespace smpp {
namespace timeformat {

std::chrono::time_point<std::chrono::system_clock> ParseDlrTimestamp(const std::string &time) {
  struct tm tm;
  tm.tm_isdst = -1;  // Set to avoid garbage.
  tm.tm_sec = 0;     // Set to avoid garbage.
  strptime(time.c_str(), "%y%m%d%H%M", &tm);
  return std::chrono::system_clock::from_time_t(mktime(&tm));
}

std::chrono::seconds ParseRelativeTimestamp(const smatch &match) {
  int yy = stoi(match[1]);
  int mon = stoi(match[2]);
  int dd = stoi(match[3]);
  int hh = stoi(match[4]);
  int min = stoi(match[5]);
  int sec = stoi(match[6]);
  int total_hours = (yy * 365 * 24) + (mon * 30 * 24) + (dd * 24) + hh;
  long total_minutes = (total_hours * 60) + min;
  long total_seconds = (total_minutes * 60) + sec;
  return std::chrono::seconds(total_seconds);
}

std::chrono::time_point<std::chrono::system_clock> ParseAbsoluteTimestamp(const std::smatch &match) {
  int yy = stoi(match[1]);
  int mon = stoi(match[2]);
  int dd = stoi(match[3]);
  int hh = stoi(match[4]);
  int min = stoi(match[5]);
  int sec = stoi(match[6]);
  // Parse timezone offset
  int n = stoi(match[8]);
  int offset_hours = (n >> 2);
  int offset_minutes = (n % 4) * 15;

  struct tm tm;
  tm.tm_year = 2000 + yy - 1900;
  tm.tm_mon = mon - 1;
  tm.tm_mday = dd;
  tm.tm_hour = hh;
  tm.tm_min = min;
  tm.tm_sec = sec;
  tm.tm_isdst = -1;
  int offset_sign = match[9].compare("+") ? 1 : -1;
  tm.tm_gmtoff = offset_sign * ((offset_hours * 60 * 60) + (offset_minutes * 60));

  return std::chrono::system_clock::from_time_t(mktime(&tm));
}

ChronoDatePair ParseSmppTimestamp(const string &time) {
  namespace sc = std::chrono;
  // Matches the pattern “YYMMDDhhmmsstnnp”
  static const regex pattern("^(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{1})(\\d{2})([R+-])$");
  smatch match;

  if (regex_match(time.begin(), time.end(), match, pattern)) {
    // relative
    if (match[match.size() - 1] == "R") {
      // parse the relative timestamp
      auto td = ParseRelativeTimestamp(match);
      // Construct a absolute timestamp based on the relative timestamp
      sc::time_point<sc::system_clock> tp = sc::system_clock::now();
      tp += td;
      return ChronoDatePair(tp, td);
    } else {
      // parse the absolute timestamp
      sc::time_point<sc::system_clock> tp = ParseAbsoluteTimestamp(match);
      // construct a relative timestamp based on the local clock and the absolute timestamp
      sc::time_point<sc::system_clock> now = sc::system_clock::now();
      auto td = sc::duration_cast<sc::seconds>(tp - now);
      return ChronoDatePair(tp, td);
    }
  }
  throw smpp::SmppException(string("Timestamp \"") + time + "\" has the wrong format.");
}

string ToSmppTimeString(const struct tm &tm) {
  int yy = tm.tm_year % 100;
  int mm = tm.tm_mon + 1;
  int dd = tm.tm_mday;
  int h = tm.tm_hour;
  int m = tm.tm_min;
  int s = tm.tm_sec;
  string p = tm.tm_gmtoff < 0 ? "-" : "+";
  // nn is time difference in quarter hours and gmtoff is in seconds
  int nn = tm.tm_gmtoff == 0 ? 0 : (std::abs(tm.tm_gmtoff) / 60)  / 15;
  char buf[16];
  //            YY  MM  DD  hh   mm   ss  000R
  sprintf(buf, "%02i%02i%02i%02i%02i%02i0%02i%s", yy, mm, dd, h, m, s, nn, p.c_str());
  return string(buf);
}

string ToSmppTimeString(const std::chrono::seconds &d) {
  namespace sc = std::chrono;
  auto tmp(d);
  auto h = sc::duration_cast<sc::hours>(tmp).count();
  tmp -= sc::hours(h);
  auto m = sc::duration_cast<sc::minutes>(tmp).count();
  tmp -= sc::minutes(m);
  auto s = sc::duration_cast<sc::seconds>(tmp).count();
  int yy = h / 24 / 365;
  h -= (yy * 24 * 365);
  int mon = h / 24 / 30;
  h -= (mon * 24 * 30);
  int dd = h / 24;
  h -= (dd * 24);
  if (yy > 99) {
    throw SmppException("Time duration overflows");
  }

  char buf[16];
  //            YY  MM  DD  hh   mm   ss  000R
  sprintf(buf, "%02i%02i%02i%02li%02li%02lli000R", yy, mon, dd, h, m, s);
  return string(buf);
}

}  // namespace timeformat
}  // namespace smpp
