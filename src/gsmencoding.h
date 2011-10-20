#ifndef GSMENCODING_H_
#define GSMENCODING_H_

#include <boost/bimap/bimap.hpp>
#include <string>
#include <sstream>

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
	GsmEncoder() :
			dict()
	{
		dict.insert(GsmDictionay::value_type("@", "\x00"));
		dict.insert(GsmDictionay::value_type("£", "\x01"));
		dict.insert(GsmDictionay::value_type("$", "\x02"));
		dict.insert(GsmDictionay::value_type("¥", "\x03"));
		dict.insert(GsmDictionay::value_type("è", "\x04"));
		dict.insert(GsmDictionay::value_type("é", "\x05"));
		dict.insert(GsmDictionay::value_type("ù", "\x06"));
		dict.insert(GsmDictionay::value_type("ì", "\x07"));
		dict.insert(GsmDictionay::value_type("ò", "\x08"));
		dict.insert(GsmDictionay::value_type("Ç", "\x09"));
		dict.insert(GsmDictionay::value_type("Ø", "\x0B"));
		dict.insert(GsmDictionay::value_type("ø", "\x0C"));
		dict.insert(GsmDictionay::value_type("Å", "\x0E"));
		dict.insert(GsmDictionay::value_type("å", "\x0F"));
		dict.insert(GsmDictionay::value_type("Δ", "\x10"));
		dict.insert(GsmDictionay::value_type("_", "\x11"));
		dict.insert(GsmDictionay::value_type("Φ", "\x12"));
		dict.insert(GsmDictionay::value_type("Γ", "\x13"));
		dict.insert(GsmDictionay::value_type("Λ", "\x14"));
		dict.insert(GsmDictionay::value_type("Ω", "\x15"));
		dict.insert(GsmDictionay::value_type("Π", "\x16"));
		dict.insert(GsmDictionay::value_type("Ψ", "\x17"));
		dict.insert(GsmDictionay::value_type("Σ", "\x18"));
		dict.insert(GsmDictionay::value_type("Θ", "\x19"));
		dict.insert(GsmDictionay::value_type("Ξ", "\x1A"));
		dict.insert(GsmDictionay::value_type("Æ", "\x1C"));
		dict.insert(GsmDictionay::value_type("æ", "\x1D"));
		dict.insert(GsmDictionay::value_type("ß", "\x1E"));
		dict.insert(GsmDictionay::value_type("É", "\x1F"));
		dict.insert(GsmDictionay::value_type("¤", "\x24"));
		dict.insert(GsmDictionay::value_type("Ä", "\x5B"));
		dict.insert(GsmDictionay::value_type("Ö", "\x5C"));
		dict.insert(GsmDictionay::value_type("Ñ", "\x5D"));
		dict.insert(GsmDictionay::value_type("Ü", "\x5E"));
		dict.insert(GsmDictionay::value_type("§", "\x5F"));
		dict.insert(GsmDictionay::value_type("¿", "\x60"));
		dict.insert(GsmDictionay::value_type("ä", "\x7B"));
		dict.insert(GsmDictionay::value_type("ö", "\x7C"));
		dict.insert(GsmDictionay::value_type("ñ", "\x7D"));
		dict.insert(GsmDictionay::value_type("ü", "\x7E"));
		dict.insert(GsmDictionay::value_type("à", "\x7F"));
		dict.insert(GsmDictionay::value_type("^", "\x1B\x14"));
		dict.insert(GsmDictionay::value_type("{", "\x1B\x28"));
		dict.insert(GsmDictionay::value_type("}", "\x1B\x29"));
		dict.insert(GsmDictionay::value_type("\\", "\x1B\x2F"));
		dict.insert(GsmDictionay::value_type("[", "\x1B\x3C"));
		dict.insert(GsmDictionay::value_type("~", "\x1B\x3D"));
		dict.insert(GsmDictionay::value_type("]", "\x1B\x3E"));
		dict.insert(GsmDictionay::value_type("|", "\x1B\x40"));
		dict.insert(GsmDictionay::value_type("€", "\x1B\x65"));
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

		GsmDictionay::left_const_iterator it;

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
				it = dict.left.find(c);
				out += it->second;
			} else if (code >= 0x20 && code <= 0x7A) { 		// 0x20 - 0x7A (except 0x40, 0x24, 0x5B-0x5F and 0x60)
				out += c[0];
			} else if (code >= 0x7B && code <= 0x7E) { 		// 0x7B - 0x7E
				it = dict.left.find(c);
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
				it = dict.left.find(s);
				if (it != dict.left.end()) {
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

		GsmDictionay::right_const_iterator it;

		char c[3];
		for (unsigned int i = 0 ; i < input.length() ; i++) {
			c[2] = c[1] = 0;
			ss.read(c, 1);
			uint8_t code = static_cast<uint8_t>(c[0]);
			if (code != 0x24 && code >= 0x20 && code <= 0x5A) { // Shortcut (avoid dictionary)
				out += c[0];
			} else if (code >= 0x61 && code <= 0x7A) { // Shortcut (avoid dictionary)
				out += c[0];
			} else {
				if (code == 0x1B) { // GSM 03.38 escape sequence read next
					ss.read(c+1, 1);
					i++;
				}
				it = dict.right.find(c);
				if (it != dict.right.end()) {
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
