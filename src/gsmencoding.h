#ifndef GSMENCODING_H_
#define GSMENCODING_H_

#include <boost/bimap/bimap.hpp>
#include <string>

namespace smpp {

typedef boost::bimaps::bimap<std::string, std::string> GsmDictionay;

/**
 * Class for encoding strings in GSM 0338.
 */
class GsmEncoder
{
private:
	GsmDictionay dict;

public:

	GsmEncoder();

	/**
	 * Returns the input string encoded in GSM 0338.
	 * @param input String to be encoded.
	 * @return Encoded string.
	 */
	std::string getGsm0338(const std::string &input);

	/**
	 * Converts an GSM 0338 encoded string into UTF8.
	 * @param input String to be encoded.
	 * @return UTF8-encoded string.
	 */
	std::string getUtf8(const std::string &input);

};
}

#endif /* GSMENCODING_H_ */
