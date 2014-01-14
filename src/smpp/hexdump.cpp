/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "smpp/hexdump.h"
#include <string>

using std::stringstream;
using std::hex;
using std::setfill;
using std::setw;

std::string oc::tools::hexdump(uint8_t* bytes, size_t length) {
    stringstream out;

    if (length == 0) {
        return out.str();
    }

    // Print bytes
    size_t pos = 0;
    char asciibytes[16];

    for (; pos < length; pos++) {
        if (pos % 16 == 0) {
            // Print ascii chars + endl
            if (pos > 0) {
                out << " |";

                for (int n = 0; n < 16; n++) {
                    out << asciibytes[n];
                }

                out << "|" << "\n";
            }

            // Print position and 1 space
            out << hex << setfill('0') << setw(8) << static_cast<uint32_t>(pos) << " ";
        }

        uint8_t curbyte = bytes[pos];

        // print 1 extra space after 8 bytes
        if (pos % 8 == 0) {
            out << " ";
        }

        // Print current byte in hex
        out << hex << setfill('0') << setw(2) << static_cast<unsigned int>(curbyte) << " ";
        // If byte is within printable range of ascii (0x20-0x7F) add it as is, otherwise add a . char.
        asciibytes[pos % 16] = (curbyte >= 0x20 && curbyte <= 0x7F) ? curbyte : '.';
    }

    // Print padding + ascii chars + endl
    int remain = (pos % 16 ? pos % 16 : 16);
    int offset = pos % 16;

    for (int n = 0; n < 16 - remain; n++) {
        // print 1 extra space after 8 bytes
        if ((n + offset) != 0 && (n + offset) % 8 == 0) {  // print a extra separator after 8 bytes
            out << " ";
        }

        // print empty hex value + space
        out << "   ";
    }

    out << " |";

    for (int n = 0; n < remain; n++) {
        out << asciibytes[n];
    }

    out << "|\n";
    // Print last position
    out << hex << setfill('0') << setw(8) << static_cast<uint32_t>(length) << "\n";
    return out.str();
}
