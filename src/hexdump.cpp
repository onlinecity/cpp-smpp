/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "hexdump.h"

void oc::tools::hexdump(std::ostream &out, const boost::shared_array<uint8_t> &bytes, size_t length)
{
	using namespace std;

	if (length == 0) return;

	// Turn off implicit flushing
	out << nounitbuf;

	// Print bytes
	size_t pos = 0;
	char asciibytes[16];
	for (; pos < length ; pos++) {
		if (pos % 16 == 0) {
			// Print ascii chars + endl
			if (pos > 0) {
				out << '|';
				for (int n = 0 ; n < 16 ; n++) {
					out << asciibytes[n];
				}
				out << '|' << endl;
			}

			// Print position
			out << hex << setfill('0') << setw(8) << static_cast<uint32_t>(pos) << ' ';
		}

		uint8_t curbyte = bytes[pos];
		// Print current byte in hex
		out << hex << setfill('0') << setw(2) << static_cast<unsigned int>(curbyte) << ' ';

		// If byte is within printable range of ascii (0x20-0x7F) add it as is, otherwise add a . char.
		asciibytes[pos % 16] = (curbyte >= 0x20 && curbyte <= 0x7F) ? curbyte : '.';
	}

	// Print padding + ascii chars + endl
	int remain = (pos % 16 ? pos % 16 : 16);
	for (int n = 0 ; n < 16 - remain ; n++) {
		out << "   ";
	}
	out << '|';
	for (int n = 0 ; n < remain ; n++) {
		out << asciibytes[n];
	}
	out << '|' << endl;

	// Flush, turn on implicit flushing and reset basefield
	out << flush << unitbuf << resetiosflags(ios_base::basefield);
}
