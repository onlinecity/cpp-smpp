#include "timeformat.h"

using namespace std;
using namespace smpp::timeformat;
using namespace boost;
using namespace boost::local_time;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace smpp {
namespace timeformat {

time_duration parseRelativeTimestamp(const smatch &match)
{
	int yy = boost::lexical_cast<int>(match[1]);
	int mon = boost::lexical_cast<int>(match[2]);
	int dd = boost::lexical_cast<int>(match[3]);
	int hh = boost::lexical_cast<int>(match[4]);
	int min = boost::lexical_cast<int>(match[5]);
	int sec = boost::lexical_cast<int>(match[6]);

	int totalHours = (yy * 365 * 24) + (mon * 30 * 24) + (dd * 24) + hh;
	time_duration td(totalHours, min, sec);
	return td;
}

local_date_time parseAbsoluteTimestamp(const smatch &match)
{
	ptime ts(from_iso_string(string("20") + match[1] + match[2] + match[3] + "T" + match[4] + match[5] + match[6]));

	int n = boost::lexical_cast<int>(match[8]);
	int offsetHours = (n >> 2);
	int offsetMinutes = (n % 4) * 15;

	// construct timezone
	stringstream gmt;
	gmt << "GMT" << match[9] << setw(2) << setfill('0') << offsetHours << ":" << setw(2) << setfill('0') << offsetMinutes;

	time_zone_ptr zone(new boost::local_time::posix_time_zone(gmt.str()));
	boost::local_time::local_date_time ldt(ts.date(), ts.time_of_day(), zone, false);

	return ldt;
}

DatePair parseSmppTimestamp(const string &time)
{
	// Matches the pattern “YYMMDDhhmmsstnnp”
	static regex pattern("^(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{1})(\\d{2})([R+-])$", regex_constants::perl);
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
			boost::local_time::local_date_time ldt = parseAbsoluteTimestamp(match);
			boost::local_time::local_date_time lt = boost::local_time::local_sec_clock::local_time(ldt.zone());
			// construct a relative timestamp based on the local clock and the absolute timestamp
			boost::local_time::local_time_period ltp(ldt, lt);
			time_duration td = ltp.length();

			return DatePair(ldt, td);
		}
	}
	throw smpp::SmppException(string("Timestamp \"") + time + "\" has the wrong format.");
}

ptime parseDlrTimestamp(const string &time)
{
	stringstream ss;
	time_input_facet *fac = new time_input_facet("%y%m%d%H%M%S"); // looks like a memleak, but it's not
	ss.imbue(std::locale(std::locale::classic(), fac));
	ss << time;
	posix_time::ptime timestamp;
	ss >> timestamp;
	return timestamp;
}

string getTimeString(const local_date_time &ldt)
{
	time_zone_ptr zone = ldt.zone();
	ptime t = ldt.local_time();
	time_duration td = t.time_of_day();
	stringstream output;
	time_duration offset = ldt.is_dst() ? zone->dst_offset() : zone->base_utc_offset();

	string p = offset.is_negative() ? "-" : "+";
	int nn = abs((offset.hours() * 4) + (offset.minutes() / 15));

	string d = to_iso_string(t.date());

	output << d.substr(2, 6) << setw(2) << setfill('0') << td.hours() << setw(2) << td.minutes() << setw(2)
			<< td.seconds() << "0" << setw(2) << nn << p;

	return output.str();
}

string getTimeString(const time_duration &td)
{
	int totalHours = td.hours();

	int yy = totalHours / 24 / 365;
	totalHours -= (yy * 24 * 365);
	int mon = totalHours / 24 / 30;
	totalHours -= (mon * 24 * 30);
	int dd = totalHours / 24;
	totalHours -= (dd * 24);

	stringstream output;

	output << setfill('0') << setw(2) << yy << setw(2) << mon << setw(2) << dd << setw(2) << totalHours << setw(2)
			<< td.minutes() << setw(2) << td.seconds() << "000R";

	return output.str();
}

}
}
