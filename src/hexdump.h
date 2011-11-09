/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef HEXDUMP_H_
#define HEXDUMP_H_

#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <boost/smart_ptr.hpp>

namespace oc
{
namespace tools
{

	void hexdump(std::ostream &out, const boost::shared_array<uint8_t> &bytes, size_t length);

} // ns: tools
} // ns: oc

#endif /* HEXDUMP_H_ */
