#ifndef TIMEFORMAT_H_
#define TIMEFORMAT_H_

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

local_time::local_date_time parseAbsoluteTimestamp(const smatch &match)
{
	for (unsigned int i = 0 ; i < match.size() ; i++) {
		cout << i << ":" << match[i] << endl;
	}

	stringstream s;
	s << "20" << match[1] << match[2] << match[3] << "T" << match[4] << match[5] << match[6];

	ptime ts(from_iso_string(s.str()));

	string st = match[8];
	int nn = atoi(st.c_str());

	int offsetHours = (nn >> 2);
	int offsetMinutes = (nn % 4) * 15;

	// construct timezone
	stringstream gmt;
	gmt << "GMT" << match[9] << setw(2) << setfill('0') << offsetHours << ":" << offsetMinutes;
	string gmtStr;
	gmt >> gmtStr;

	cout << "gmtStr:" << gmtStr << endl;

	local_time::time_zone_ptr zone(new local_time::posix_time_zone(gmtStr));
	local_time::local_date_time ldt(ts, zone);

	return ldt;
}

DatePair convert(const string &time)
{
	// Matches the pattern“YYMMDDhhmmsstnnp”
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
		throw std::exception();
	}
}

/**

 * @param time
 * @return
 */
posix_time::ptime parseDlrTimeStamp(const string &time)
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
