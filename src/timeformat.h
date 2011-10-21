/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef TIMEFORMAT_H_
#define TIMEFORMAT_H_

#include "exceptions.h"
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>

namespace smpp {
namespace timeformat {

typedef std::pair<boost::local_time::local_date_time, boost::posix_time::time_duration> DatePair;

/**
 * Parses a relative timestamp and returns it as a time_duration.
 * @param match Relative timestamp on the form of a boost::regex::smatch
 * @return time_duration representation of the timestamp.
 */
boost::posix_time::time_duration parseRelativeTimestamp(const boost::smatch &match);

/**
 * Parses an absolute timestamp and returns it as a local_date_time.
 * @param match Absolute timestmp on the form of a boost::regex::smatch
 * @return local_date_time representation of the timestamp.
 */
boost::local_time::local_date_time parseAbsoluteTimestamp(const boost::smatch &match);

/**
 * Parses a smpp timestamp and returns a DatePair representation of the timestamp.
 * The smpp timestamp has the following format: “YYMMDDhhmmsstnnp”, an SmppException
 * thrown if the input timeformat differs.
 * @param time
 * @return
 * @throw SmppException.
 */
DatePair parseSmppTimestamp(const std::string &time);

/**
 * Parses a delivery receipt timestamp and returns it as ptime.
 * @param time Timestamp to parse.
 * @return ptime representation of the timestamp.
 */
boost::posix_time::ptime parseDlrTimestamp(const std::string &time);

/**
 * Returns the local_date_time as a string formatted as an absolute timestamp
 * @param ldt
 * @return
 */
std::string getTimeString(const boost::local_time::local_date_time &ldt);
/**
 * Returns a relative timestamp created from the time_duration. Since a time_duration does not handle
 * dates, the relative dates is calculated with the assumption of one year equals 365 days and a month is 30 days long.
 * @param td time_duration to be calculated.
 * @return
 */
std::string getTimeString(const boost::posix_time::time_duration &td);

} // timeformat
} // smpp

#endif /* TIMEFORMAT_H_ */
