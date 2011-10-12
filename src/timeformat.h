#ifndef TIMEFORMAT_H_
#define TIMEFORMAT_H_

#include <boost/regex.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace smpp {
namespace timeformat {

ptime convert(const string &time)
{
	// Matches the pattern“YYMMDDhhmmsstnnp”
	regex pattern("^(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{1})(\\d{2})([R+-])$", regex_constants::perl);
	smatch match;

	if (regex_match(time.begin(), time.end(), match, pattern)) {
		cout << "match" << endl;
		cout << (match) << endl;
		for (int i = 1 ; i < match.size() ; i++) {
			cout << match[i] << endl;
		}

		if (match[match.size() - 1] == "R") {
			cout << "relative" << endl;
			stringstream ss(match[1]);
			long yy;
			ss >> yy;
			ss.clear();

			long mon;
			ss.str(match[2]);
			ss >> mon;
			ss.clear();

			long dd;
			ss.str(match[3]);
			ss >> dd;
			ss.clear();

			long hh;
			ss.str(match[4]);
			ss >> hh;
			ss.clear();

			long min;
			ss.str(match[5]);
			ss >> min;

			long sec;
			ss.str(match[6]);
			ss >> sec;

			ptime now(second_clock::local_time());
			ptime relative = now + years(yy) + months(mon) + days(dd) + hours(hh) + minutes(min) + seconds(sec);
			cout << "now      :" << now << endl;
			cout << "relative :" << relative << endl;

		}

	}

	posix_time::ptime test;
	return test;
}
}
}

#endif /* TIMEFORMAT_H_ */
