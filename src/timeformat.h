#ifndef TIMEFORMAT_H_
#define TIMEFORMAT_H_

#include "exceptions.h"
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace smpp {
namespace timeformat {

typedef std::pair<boost::local_time::local_date_time, boost::posix_time::time_duration> DatePair;

/**
 * Parses a relative timestamp and returns it as a time_duration.
 * @param match Relative timestamp on the form of a boost::regex::smatch
 * @return time_duration representation of the timestamp.
 */
<<<<<<< HEAD
boost::posix_time::time_duration parseRelativeTimestamp(const boost::smatch &match);
=======
time_duration parseRelativeTimestamp(const smatch &match)
{
	stringstream ss;

	ss << match[1];
	int yy;
	ss >> yy;
	ss.clear();

	int mon;
	ss.str("");
	ss << match[2];
	ss >> mon;
	ss.clear();

	int dd;
	ss.str("");
	ss << match[3];
	ss >> dd;
	ss.clear();

	int hh;
	ss.str("");
	ss << match[4];
	ss >> hh;
	ss.clear();

	int min;
	ss.str("");
	ss << match[5];
	ss >> min;
	ss.clear();

	int sec;
	ss.str("");
	ss << match[6];
	ss >> sec;

	int totalHours = (yy * 365 * 24) + (mon * 30 * 24) + (dd * 24) + hh;
	time_duration td(totalHours, min, sec);
	return td;
}
>>>>>>> fc7693a7feb3e65d515d374d69b566217139ca9f

/**
 * Parses an absolute timestamp and returns it as a local_date_time.
 * @param match Absolute timestmp on the form of a boost::regex::smatch
 * @return local_date_time representation of the timestamp.
 */
<<<<<<< HEAD
boost::local_time::local_date_time parseAbsoluteTimestamp(const boost::smatch &match);
=======
local_time::local_date_time parseAbsoluteTimestamp(const smatch &match)
{
	stringstream ss;
	ss << "20" << match[1] << match[2] << match[3] << "T" << match[4] << match[5] << match[6];

	ptime ts(from_iso_string(ss.str()));

	string s = match[8];
	int n = atoi(s.c_str());
	int offsetHours = (n >> 2);
	int offsetMinutes = (n % 4) * 15;

	// construct timezone
	stringstream gmt;
	gmt << "GMT" << match[9] << setw(2) << setfill('0') << offsetHours << ":" << setw(2) << setfill('0') << offsetMinutes;
	local_time::time_zone_ptr zone(new local_time::posix_time_zone(gmt.str()));
	local_time::local_date_time ldt(ts, zone);

	return ldt;
}
>>>>>>> fc7693a7feb3e65d515d374d69b566217139ca9f

/**
 * Parses a smpp timestamp and returns a DatePair representation of the timestamp.
 * The smpp timestamp has the following format: “YYMMDDhhmmsstnnp”, an SmppException
 * thrown if the input timeformat differs.
 * @param time
 * @return
 * @throw SmppException.
 */
<<<<<<< HEAD
DatePair parseSmppTimestamp(const std::string &time);
=======
DatePair parseSmppTimestamp(const string &time)
{
	// Matches the pattern “YYMMDDhhmmsstnnp”
	regex pattern("^(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{1})(\\d{2})([R+-])$", regex_constants::perl);
	smatch match;
	if (regex_match(time.begin(), time.end(), match, pattern)) {

		// relative
		if (match[match.size() - 1] == "R") {

			time_duration td = parseRelativeTimestamp(match);

			local_time::time_zone_ptr zone(new local_time::posix_time_zone("GMT"));
			local_time::local_date_time ldt = local_time::local_sec_clock::local_time(zone);

			ldt += td;
			return DatePair(ldt, td);

		} else {
			local_time::local_date_time ldt = parseAbsoluteTimestamp(match);
			local_time::local_date_time lt = local_time::local_sec_clock::local_time(ldt.zone());

			local_time::local_time_period ltp(ldt, lt);
			time_duration td = ltp.length();

			return DatePair(ldt, td);
		}
	} else {
		stringstream ss;
		ss << "Timestamp \"" << time << "\" has the wrong format.";
		throw smpp::SmppException(ss.str());
	}
}
>>>>>>> fc7693a7feb3e65d515d374d69b566217139ca9f

/**
 * Parses a delivery receipt timestamp and returns it as ptime.
 * @param time Timestamp to parse.
 * @return ptime representation of the timestamp.
 */
<<<<<<< HEAD
boost::posix_time::ptime parseDlrTimestamp(const std::string &time);

/**
 * Returns the local_date_time as a string formatted as an absolute timestamp
 * @param ldt
 * @return
 */
std::string getTimeString(const boost::local_time::local_date_time ldt);
/**
 * Returns a relative timestamp created from the time_duration. Since a time_duration does not handle
 * dates, the relative dates is calculated with the assumption of one year equals 365 days and a month is 30 days long.
 * @param td time_duration to be calculated.
 * @return
 */
std::string getTimeString(const boost::posix_time::time_duration &td);
=======
posix_time::ptime parseDlrTimestamp(const string &time)
{
	stringstream ss;
	time_input_facet *fac = new time_input_facet("%y%m%d%H%M%S");
	ss.imbue(std::locale(std::locale::classic(), fac));
	ss << time;
	posix_time::ptime timestamp;
	ss >> timestamp;
	return timestamp;
}
>>>>>>> fc7693a7feb3e65d515d374d69b566217139ca9f

} // timeformat
} // smpp

#endif /* TIMEFORMAT_H_ */
