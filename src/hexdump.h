/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef HEXDUMP_H_
#define HEXDUMP_H_

#include <sstream>
#include <iomanip>
#include <stdint.h>

namespace oc {
namespace tools {
/**
 * Returns a string, of the input byte array, that resembles the output of the GNU hexdump program.
 * @param bytes Byte array to be printed.
 * @param length Length of the byte array.
 * @return
 */
std::string hexdump(uint8_t *bytes, size_t length);

} // namespace tools
} // namespace oc

#endif /* HEXDUMP_H_ */
