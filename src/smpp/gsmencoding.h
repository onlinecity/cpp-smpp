/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_GSMENCODING_H_
#define SMPP_GSMENCODING_H_

#include <boost/bimap/bimap.hpp>
#include <string>

namespace oc {
namespace tools {
typedef boost::bimaps::bimap<std::string, std::string> GsmDictionary;

/**
 * Class for encoding strings in GSM 0338.
 * It's a singleton so call getInstance.
 */
class GsmEncoder {
  public:
    /**
     * Returns the input string encoded in GSM 0338.
     * @param input String to be encoded.
     * @return Encoded string.
     */
    static std::string getGsm0338(const std::string &input);

    /**
     * Converts an GSM 0338 encoded string into UTF8.
     * @param input String to be encoded.
     * @return UTF8-encoded string.
     */
    static std::string getUtf8(const std::string &input);
};
}  // namespace tools
}  // namespace oc

#endif  // SMPP_GSMENCODING_H_
