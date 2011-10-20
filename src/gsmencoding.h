#ifndef GSMENCODING_H_
#define GSMENCODING_H_

#include <map>
#include <string>
#include <sstream>

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
	 * @param input String to be encoded.
	 * @return Encoded string.
	 */
	std::string getGsm0338(const std::string &input)
	{
		std::stringstream ss(input);

		std::string out;

		std::map<std::string, std::string>::iterator it;

		char c[4];
		for (unsigned int i = 0 ; i < input.length() ; i++) {
			c[3] = c[2] = c[1] = c[0] = 0; // reset array
			ss.read(c, 1);
			uint8_t code = static_cast<uint8_t>(c[0]);
			if (code == 0x40) { 							// @
				out += '\0';
			} else if (code == 0x60) { 						// `
				out += '?';
			} else if (code == 0x24) { 						// $
				out += "\x02";
			} else if (code >= 0x5B && code <= 0x5F) { 		// 0x5B - 0x5F
				it = dict.find(c);
				out += it->second;
			} else if (code >= 0x20 && code <= 0x7A) { 		// 0x20 - 0x7A (except 0x40, 0x24, 0x5B-0x5F and 0x60)
				out += c[0];
			} else if (code >= 0x7B && code <= 0x7E) { 		// 0x7B - 0x7E
				it = dict.find(c);
				out += it->second;
			} else if (code >= 0x7F) { 						// UTF-8 escape sequence
				std::string s;
				if (code >= 0xC0 && code <= 0xDF) { 		// Double byte UTF-8
					ss.read(c + 1, 1);
					i++;
					s = std::string(c, 2);
				} else if (code >= 0xE0 && code <= 0xF0) { 	// Triple byte UTF-8
					ss.read(c + 1, 2);
					i += 2;
					s = std::string(c, 3);
				} else { 									// Quad byte UTF-8
					ss.read(c + 1, 3);
					i += 3;
					s = std::string(c, 4);
				}
				it = dict.find(s);
				if (it != dict.end()) {
					out += it->second;
				} else {
					out += '?';
				}
			} else { 										// Unprintable char: ignore

			}
		}
		return out;
	}

	/**
	 * Converts an GSM 0338 encoded string into UTF8.
	 * @param input String to be encoded.
	 * @return UTF8-encoded string.
	 */
	std::string getUtf8(const std::string &input)
	{
		std::stringstream ss(input);
		std::string out;
		std::map<std::string, std::string>::iterator it;

		char c[1];
		for (unsigned int i = 0 ; i < input.length() ; i++) {
			ss.read(c, 1);
			uint8_t code = static_cast<uint8_t>(c[0]);
			if (code >= 0x20 && code <= 0x5A) {
				out += c[0];
			} else {
				it = dict.find(c);
				if (it != dict.end()) {
					out += it->second;
				} else {
					out += c[0];
				}
			}
		}

		return out;
	}
};
}

#endif /* GSMENCODING_H_ */
