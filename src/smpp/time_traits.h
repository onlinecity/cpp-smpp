//
// Copyright (C) 2014 - OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// Stackoverflow reference:
// http://stackoverflow.com/questions/16721243/boostasiodeadline-timer-with-stdchrono-time-values
//

#ifndef SMPP_TIME_TRAITS_H_
#define SMPP_TIME_TRAITS_H_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>

namespace smpp {
template<typename Clock>
struct CXX11Traits {
  typedef typename Clock::time_point time_type;
  typedef typename Clock::duration duration_type;

  static time_type now() {
    return Clock::now();
  }

  static time_type add(time_type t, duration_type d) {
    return t + d;
  }

  static duration_type subtract(time_type t1, time_type t2) {
    return t1 - t2;
  }

  static bool less_than(time_type t1, time_type t2) {
    return t1 < t2;
  }

  static boost::posix_time::time_duration to_posix_duration(duration_type d1) {
    using std::chrono::duration_cast;
    auto in_sec = duration_cast<std::chrono::seconds>(d1);
    auto in_usec = duration_cast<std::chrono::microseconds>(d1 - in_sec);
    boost::posix_time::time_duration result =
      boost::posix_time::seconds(in_sec.count()) +
      boost::posix_time::microseconds(in_usec.count());
    return result;
  }
};
}  // namespace smpp

#endif  // SMPP_TIME_TRAITS_H_
