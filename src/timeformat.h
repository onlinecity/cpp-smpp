#ifndef TIMEFORMAT_H_
#define TIMEFORMAT_H_

#include "exceptions.h"
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace smpp {
namespace timeformat {

typedef pair<local_time::local_date_time, posix_time::time_duration> DatePair;

/**
 * Parses a relative timestamp and returns it as a time_duration.
 * @param match Relative timestamp on the form of a boost::regex::smatch
 * @return time_duration representation of the timestamp.
 */
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

/**
 * Parses an absolute timestamp and returns it as a local_date_time.
 * @param match Absolute timestmp on the form of a boost::regex::smatch
 * @return local_date_time representation of the timestamp.
 */
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

/**
 * Parses a smpp timestamp and returns a DatePair representation of the timestamp.
 * The smpp timestamp has the following format: “YYMMDDhhmmsstnnp”, an SmppException
 * thrown if the input timeformat differs.
 * @param time
 * @return
 * @throw SmppException.
 */
DatePair parseSmppTimestamp(const string &time)
{
	// Matches the pattern “YYMMDDhhmmsstnnp”
	regex pattern("^(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{1})(\\d{2})([R+-])$", regex_constants::perl);
	smatch match;
	if (regex_match(time.begin(), time.end(), match, pattern)) {

		// relative
		if (match[match.size() - 1] == "R") {
			// parse the relative timestamp
			time_duration td = parseRelativeTimestamp(match);
			// construct a absolute timestamp based on the relative timestamp
			local_time::time_zone_ptr zone(new local_time::posix_time_zone("GMT"));
			local_time::local_date_time ldt = local_time::local_sec_clock::local_time(zone);
			ldt += td;

			return DatePair(ldt, td);
		} else {
			// parse the absolute timestamp
			local_time::local_date_time ldt = parseAbsoluteTimestamp(match);
			local_time::local_date_time lt = local_time::local_sec_clock::local_time(ldt.zone());
			// construct a relative timestamp based on the local clock and the absolute timestamp
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

/**
 * Parses a delivery receipt timestamp and returns it as ptime.
 * @param time Timestamp to parse.
 * @return ptime representation of the timestamp.
 */
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

} // timeformat
} // smpp

#endif /* TIMEFORMAT_H_ */
