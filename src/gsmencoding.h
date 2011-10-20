#ifndef GSMENCODING_H_
#define GSMENCODING_H_

#include <map>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace smpp {
/**
 * Class for encoding strings in GSM 0338.
 */
class GsmEncoder
{
private:
	std::map<std::string, std::string> dict;

public:
	GsmEncoder() :
			dict()
	{
		dict.insert(std::pair<std::string, std::string>("@", "\x00"));
		dict.insert(std::pair<std::string, std::string>("£", "\x01"));
		dict.insert(std::pair<std::string, std::string>("$", "\x02"));
		dict.insert(std::pair<std::string, std::string>("¥", "\x03"));
		dict.insert(std::pair<std::string, std::string>("è", "\x04"));
		dict.insert(std::pair<std::string, std::string>("é", "\x05"));
		dict.insert(std::pair<std::string, std::string>("ù", "\x06"));
		dict.insert(std::pair<std::string, std::string>("ì", "\x07"));
		dict.insert(std::pair<std::string, std::string>("ò", "\x08"));
		dict.insert(std::pair<std::string, std::string>("Ç", "\x09"));
		dict.insert(std::pair<std::string, std::string>("Ø", "\x0B"));
		dict.insert(std::pair<std::string, std::string>("ø", "\x0C"));
		dict.insert(std::pair<std::string, std::string>("Å", "\x0E"));
		dict.insert(std::pair<std::string, std::string>("å", "\x0F"));
		dict.insert(std::pair<std::string, std::string>("Δ", "\x10"));
		dict.insert(std::pair<std::string, std::string>("_", "\x11"));
		dict.insert(std::pair<std::string, std::string>("Φ", "\x12"));
		dict.insert(std::pair<std::string, std::string>("Γ", "\x13"));
		dict.insert(std::pair<std::string, std::string>("Λ", "\x14"));
		dict.insert(std::pair<std::string, std::string>("Ω", "\x15"));
		dict.insert(std::pair<std::string, std::string>("Π", "\x16"));
		dict.insert(std::pair<std::string, std::string>("Ψ", "\x17"));
		dict.insert(std::pair<std::string, std::string>("Σ", "\x18"));
		dict.insert(std::pair<std::string, std::string>("Θ", "\x19"));
		dict.insert(std::pair<std::string, std::string>("Ξ", "\x1A"));
		dict.insert(std::pair<std::string, std::string>("Æ", "\x1C"));
		dict.insert(std::pair<std::string, std::string>("æ", "\x1D"));
		dict.insert(std::pair<std::string, std::string>("ß", "\x1E"));
		dict.insert(std::pair<std::string, std::string>("É", "\x1F"));
		// all \x2? removed
		// all \x3? removed
		// all \x4? removed
		dict.insert(std::pair<std::string, std::string>("Ä", "\x5B"));
		dict.insert(std::pair<std::string, std::string>("Ö", "\x5C"));
		dict.insert(std::pair<std::string, std::string>("Ñ", "\x5D"));
		dict.insert(std::pair<std::string, std::string>("Ü", "\x5E"));
		dict.insert(std::pair<std::string, std::string>("§", "\x5F"));
		dict.insert(std::pair<std::string, std::string>("¿", "\x60"));
		dict.insert(std::pair<std::string, std::string>("ä", "\x7B"));
		dict.insert(std::pair<std::string, std::string>("ö", "\x7C"));
		dict.insert(std::pair<std::string, std::string>("ñ", "\x7D"));
		dict.insert(std::pair<std::string, std::string>("ü", "\x7E"));
		dict.insert(std::pair<std::string, std::string>("à", "\x7F"));
		dict.insert(std::pair<std::string, std::string>("^", "\x1B\x14"));
		dict.insert(std::pair<std::string, std::string>("{", "\x1B\x28"));
		dict.insert(std::pair<std::string, std::string>("}", "\x1B\x29"));
		dict.insert(std::pair<std::string, std::string>("\\", "\x1B\x2F"));
		dict.insert(std::pair<std::string, std::string>("[", "\x1B\x3C"));
		dict.insert(std::pair<std::string, std::string>("~", "\x1B\x3D"));
		dict.insert(std::pair<std::string, std::string>("]", "\x1B\x3E"));
		dict.insert(std::pair<std::string, std::string>("|", "\x1B\x40"));
		dict.insert(std::pair<std::string, std::string>("€", "\x1B\x65"));
	}

	/**
	 * Returns the input string encoded in GSM 0338.
	 * @param str String to be encoded.
	 * @return Encoded string.
	 */
	std::string getGsm0338(std::string &str)
	{
		std::map<std::string, std::string>::iterator it = dict.begin();
		for (; it != dict.end() ; it++) {
			boost::replace_all(str, it->first, it->second);
		}

		static const boost::regex pattern("([\\xC0-\\xDF].)|([\\xE0-\\xEF]..)|([\\xF0-\\xFF]...)", boost::regex_constants::perl);
		return boost::regex_replace(str, pattern, "?");
	}

	/**
	 * Converts an GSM 0338 encoded string into UTF8.
	 * @param str String to be encoded.
	 * @return UTF8-encoded string.
	 */
	std::string getUtf8(std::string &str)
	{
		std::map<std::string, std::string>::iterator it = dict.begin();
		for (; it != dict.end() ; it++) {
			boost::replace_all(str, it->second, it->first);
		}

		return str;
	}
};
}

#endif /* GSMENCODING_H_ */
