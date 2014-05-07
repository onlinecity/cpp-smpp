// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace oc {
namespace tools {
// Returns a string, of the input byte array, that resembles the output of the GNU hexdump program.
// @param bytes Byte array to be printed.
// @param length Length of the byte array.
// @return
std::string hexdump(const unsigned char *bytes, size_t length);

}  // namespace tools
}  // namespace oc
