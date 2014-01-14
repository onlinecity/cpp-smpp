/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "smpp/timeformat.h"
#include <string>

using std::setfill;
using std::setw;
using std::string;
using std::stoi;
using std::stringstream;

using boost::local_time::local_date_time;
using boost::local_time::posix_time_zone;
using boost::local_time::time_zone_ptr;
using boost::posix_time::from_iso_string;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::posix_time::time_input_facet;

using std::regex;
using std::smatch;

namespace smpp {
namespace timeformat {
time_duration parseRelativeTimestamp(const smatch &match) {
    int yy = stoi(match[1]);
    int mon = stoi(match[2]);
    int dd = stoi(match[3]);
    int hh = stoi(match[4]);
    int min = stoi(match[5]);
    int sec = stoi(match[6]);
    int totalHours = (yy * 365 * 24) + (mon * 30 * 24) + (dd * 24) + hh;
    time_duration td(totalHours, min, sec);
    return td;
}

local_date_time parseAbsoluteTimestamp(const smatch &match) {
    string yy = match[1];
    string mon = match[2];
    string dd = match[3];
    string hh = match[4];
    string min = match[5];
    string sec = match[6];
    ptime ts(from_iso_string(string("20") + yy + mon + dd + "T" + hh + min + sec));
    int n = stoi(match[8]);
    int offsetHours = (n >> 2);
    int offsetMinutes = (n % 4) * 15;
    // construct timezone
    stringstream gmt;
    gmt << "GMT" << match[9] << setw(2) << setfill('0') << offsetHours << ":" << setw(2) << setfill('0')
        << offsetMinutes;
    time_zone_ptr zone(new boost::local_time::posix_time_zone(gmt.str()));
    boost::local_time::local_date_time ldt(ts.date(), ts.time_of_day(), zone, false);
    return ldt;
}

DatePair parseSmppTimestamp(const string &time) {
    // Matches the pattern “YYMMDDhhmmsstnnp”
    static const regex pattern("^(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{1})(\\d{2})([R+-])$");
    smatch match;

    if (regex_match(time.begin(), time.end(), match, pattern)) {
        // relative
        if (match[match.size() - 1] == "R") {
            // parse the relative timestamp
            time_duration td = parseRelativeTimestamp(match);
            // construct a absolute timestamp based on the relative timestamp
            time_zone_ptr zone(new posix_time_zone("GMT"));
            local_date_time ldt = boost::local_time::local_sec_clock::local_time(zone);
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

ptime parseDlrTimestamp(const string &time) {
    stringstream ss;
    time_input_facet* fac = new time_input_facet("%y%m%d%H%M%S");  // looks like a memleak, but it's not
    ss.imbue(std::locale(std::locale::classic(), fac));
    ss << time;
    ptime timestamp;
    ss >> timestamp;
    return timestamp;
}

string getTimeString(const local_date_time &ldt) {
    time_zone_ptr zone = ldt.zone();
    ptime t = ldt.local_time();
    time_duration td = t.time_of_day();
    stringstream output;
    time_duration offset = zone->base_utc_offset();

    if (ldt.is_dst()) {
        offset += zone->dst_offset();
    }

    string p = offset.is_negative() ? "-" : "+";
    int nn = abs((offset.hours() * 4) + (offset.minutes() / 15));
    string d = to_iso_string(t.date());
    output << d.substr(2, 6) << setw(2) << setfill('0') << td.hours() << setw(2) << td.minutes() << setw(2)
           << td.seconds() << "0" << setw(2) << nn << p;
    return output.str();
}

string getTimeString(const time_duration &td) {
    int totalHours = td.hours();
    int yy = totalHours / 24 / 365;
    totalHours -= (yy * 24 * 365);
    int mon = totalHours / 24 / 30;
    totalHours -= (mon * 24 * 30);
    int dd = totalHours / 24;
    totalHours -= (dd * 24);

    if (yy > 99) {
        throw SmppException("Time duration overflows");
    }

    stringstream output;
    output << setfill('0') << setw(2) << yy << setw(2) << mon << setw(2) << dd << setw(2) << totalHours << setw(2)
           << td.minutes() << setw(2) << td.seconds() << "000R";
    return output.str();
}
}  // namespace timeformat
}  // namespace smpp
