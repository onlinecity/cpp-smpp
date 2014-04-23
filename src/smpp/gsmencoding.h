/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_GSMENCODING_H_
#define SMPP_GSMENCODING_H_

#include <string>
#include <unordered_map>

namespace oc {
namespace tools {
/**
 * Class for encoding strings in GSM 0338.
 */
class GsmEncoder {
 public:
  /**
   * Returns the input string encoded in GSM 0338.
   * @param input String to be encoded.
   * @return Encoded string.
   */
  static std::string EncodeGsm0338(const std::string &input);

  /**
   * Converts an GSM 0338 encoded string into UTF8.
   * @param input String to be encoded.
   * @return UTF8-encoded string.
   */
  static std::string EncodeUtf8(const std::string &input);
 private:
  static const std::unordered_map<std::string, std::string> gsm0338_map_;
  static const std::unordered_map<std::string, std::string> utf8_map_;
};
}  // namespace tools
}  // namespace oc

#endif  // SMPP_GSMENCODING_H_
