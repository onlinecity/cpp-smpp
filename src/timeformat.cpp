#include "timeformat.h"
using namespace smpp::timeformat;

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
	local_time::local_date_time ldt(ts.date(), ts.time_of_day(), zone, false);
	return ldt;
}

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

string getTimeString(const local_time::local_date_time ldt)
{
	using namespace boost::local_time;

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
	using namespace boost::local_time;

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

