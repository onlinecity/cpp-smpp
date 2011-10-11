#ifndef GSMENCODING_H_
#define GSMENCODING_H_

#include <map>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

namespace smpp {
/**
 * Class for encoding strings in GSM 0338.
 */
class GsmEncoder
{
private:
	map<string, string> dict;

public:
	GsmEncoder() :
			dict()
	{
		dict.insert(pair<string, string>("@", "\x00"));
		dict.insert(pair<string, string>("£", "\x01"));
		dict.insert(pair<string, string>("$", "\x02"));
		dict.insert(pair<string, string>("¥", "\x03"));
		dict.insert(pair<string, string>("è", "\x04"));
		dict.insert(pair<string, string>("é", "\x05"));
		dict.insert(pair<string, string>("ù", "\x06"));
		dict.insert(pair<string, string>("ì", "\x07"));
		dict.insert(pair<string, string>("ò", "\x08"));
		dict.insert(pair<string, string>("Ç", "\x09"));
		dict.insert(pair<string, string>("Ø", "\x0B"));
		dict.insert(pair<string, string>("ø", "\x0C"));
		dict.insert(pair<string, string>("Å", "\x0E"));
		dict.insert(pair<string, string>("å", "\x0F"));
		dict.insert(pair<string, string>("Δ", "\x10"));
		dict.insert(pair<string, string>("_", "\x11"));
		dict.insert(pair<string, string>("Φ", "\x12"));
		dict.insert(pair<string, string>("Γ", "\x13"));
		dict.insert(pair<string, string>("Λ", "\x14"));
		dict.insert(pair<string, string>("Ω", "\x15"));
		dict.insert(pair<string, string>("Π", "\x16"));
		dict.insert(pair<string, string>("Ψ", "\x17"));
		dict.insert(pair<string, string>("Σ", "\x18"));
		dict.insert(pair<string, string>("Θ", "\x19"));
		dict.insert(pair<string, string>("Ξ", "\x1A"));
		dict.insert(pair<string, string>("Æ", "\x1C"));
		dict.insert(pair<string, string>("æ", "\x1D"));
		dict.insert(pair<string, string>("ß", "\x1E"));
		dict.insert(pair<string, string>("É", "\x1F"));
		// all \x2? removed
		// all \x3? removed
		// all \x4? removed
		dict.insert(pair<string, string>("Ä", "\x5B"));
		dict.insert(pair<string, string>("Ö", "\x5C"));
		dict.insert(pair<string, string>("Ñ", "\x5D"));
		dict.insert(pair<string, string>("Ü", "\x5E"));
		dict.insert(pair<string, string>("§", "\x5F"));
		dict.insert(pair<string, string>("¿", "\x60"));
		dict.insert(pair<string, string>("ä", "\x7B"));
		dict.insert(pair<string, string>("ö", "\x7C"));
		dict.insert(pair<string, string>("ñ", "\x7D"));
		dict.insert(pair<string, string>("ü", "\x7E"));
		dict.insert(pair<string, string>("à", "\x7F"));
		dict.insert(pair<string, string>("^", "\x1B\x14"));
		dict.insert(pair<string, string>("{", "\x1B\x28"));
		dict.insert(pair<string, string>("}", "\x1B\x29"));
		dict.insert(pair<string, string>("\\", "\x1B\x2F"));
		dict.insert(pair<string, string>("[", "\x1B\x3C"));
		dict.insert(pair<string, string>("~", "\x1B\x3D"));
		dict.insert(pair<string, string>("]", "\x1B\x3E"));
		dict.insert(pair<string, string>("|", "\x1B\x40"));
		dict.insert(pair<string, string>("€", "\x1B\x65"));
	}

	/**
	 * Returns the input string encoded in GSM 0338.
	 * @param str String to be encoded.
	 * @return Encoded string.
	 */
	string getGsm0338(string &str)
	{
		map<string, string>::iterator it = dict.begin();
		for (; it != dict.end() ; it++) {
			replace_all(str, it->first, it->second);
		}

		const regex pattern("([\\xC0-\\xDF].)|([\\xE0-\\xEF]..)|([\\xF0-\\xFF]...)", regex_constants::perl);
		return regex_replace(str, pattern, "?");
	}

	/**
	 * Converts an GSM 0338 encoded string into UTF8.
	 * @param str String to be encoded.
	 * @return UTF8-encoded string.
	 */
	string getUtf8(string &str)
	{
		map<string, string>::iterator it = dict.begin();
		for (; it != dict.end() ; it++) {
			replace_all(str, it->second, it->first);
		}

		return str;
	}
};
}

#endif /* GSMENCODING_H_ */
