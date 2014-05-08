// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#include "smpp/gsmencoding.h"
#include <string>
#include <unordered_map>

namespace smpp {
namespace encoding {
using std::string;
const std::unordered_map<std::string, std::string> GsmEncoder::gsm0338_map_ = {
  {"@", "\x00"},
  {"£", "\x01"},
  {"$", "\x02"},
  {"¥", "\x03"},
  {"è", "\x04"},
  {"é", "\x05"},
  {"ù", "\x06"},
  {"ì", "\x07"},
  {"ò", "\x08"},
  {"Ç", "\x09"},
  {"Ø", "\x0B"},
  {"ø", "\x0C"},
  {"Å", "\x0E"},
  {"å", "\x0F"},
  {"Δ", "\x10"},
  {"_", "\x11"},
  {"Φ", "\x12"},
  {"Γ", "\x13"},
  {"Λ", "\x14"},
  {"Ω", "\x15"},
  {"Π", "\x16"},
  {"Ψ", "\x17"},
  {"Σ", "\x18"},
  {"Θ", "\x19"},
  {"Ξ", "\x1A"},
  {"Æ", "\x1C"},
  {"æ", "\x1D"},
  {"ß", "\x1E"},
  {"É", "\x1F"},
  {"¤", "\x24"},
  {"Ä", "\x5B"},
  {"Ö", "\x5C"},
  {"Ñ", "\x5D"},
  {"Ü", "\x5E"},
  {"§", "\x5F"},
  {"¿", "\x60"},
  {"ä", "\x7B"},
  {"ö", "\x7C"},
  {"ñ", "\x7D"},
  {"ü", "\x7E"},
  {"à", "\x7F"},
  {"^", "\x1B\x14"},
  {"{", "\x1B\x28"},
  {"}", "\x1B\x29"},
  {"\\", "\x1B\x2F"},
  {"[", "\x1B\x3C"},
  {"~", "\x1B\x3D"},
  {"]", "\x1B\x3E"},
  {"|", "\x1B\x40"},
  {"€", "\x1B\x65"}
};

const std::unordered_map<std::string, std::string> GsmEncoder::utf8_map_ = {
  {"\x00", "@"},
  {"\x01", "£"},
  {"\x02", "$"},
  {"\x03", "¥"},
  {"\x04", "è"},
  {"\x05", "é"},
  {"\x06", "ù"},
  {"\x07", "ì"},
  {"\x08", "ò"},
  {"\x09", "Ç"},
  {"\x0B", "Ø"},
  {"\x0C", "ø"},
  {"\x0E", "Å"},
  {"\x0F", "å"},
  {"\x10", "Δ"},
  {"\x11", "_"},
  {"\x12", "Φ"},
  {"\x13", "Γ"},
  {"\x14", "Λ"},
  {"\x15", "Ω"},
  {"\x16", "Π"},
  {"\x17", "Ψ"},
  {"\x18", "Σ"},
  {"\x19", "Θ"},
  {"\x1A", "Ξ"},
  {"\x1C", "Æ"},
  {"\x1D", "æ"},
  {"\x1E", "ß"},
  {"\x1F", "É"},
  {"\x24", "¤"},
  {"\x5B", "Ä"},
  {"\x5C", "Ö"},
  {"\x5D", "Ñ"},
  {"\x5E", "Ü"},
  {"\x5F", "§"},
  {"\x60", "¿"},
  {"\x7B", "ä"},
  {"\x7C", "ö"},
  {"\x7D", "ñ"},
  {"\x7E", "ü"},
  {"\x7F", "à"},
  {"\x1B\x14", "^"},
  {"\x1B\x28", "{"},
  {"\x1B\x29", "}"},
  {"\x1B\x2F", "\\"},
  {"\x1B\x3C", "["},
  {"\x1B\x3D", "~"},
  {"\x1B\x3E", "]"},
  {"\x1B\x40", "|"},
  {"\x1B\x65", "€"},
};

string GsmEncoder::EncodeGsm0338(const string &input) {
  string out;
  // GSM 03.38 encoding will always result in equal or less chars, so resize to input length
  out.reserve(input.length());

  for (unsigned int i = 0; i < input.length(); i++) {
    uint8_t code = static_cast<uint8_t>(input[i]);
    if (code == 0x40) {  // @
      out += '\0';
    } else if (code == 0x60) {  // `
      out += '?';
    } else if (code == 0x24) {  // $
      out += "\x02";
    } else if (code >= 0x5B && code <= 0x5F) {  // 0x5B-05F
      char c[] = { input[i], '\0' };
      auto it = gsm0338_map_.find(c);

      if (it == gsm0338_map_.end()) {  // just in case
        out += input[i];
      } else {
        out += it->second;
      }
    } else if (code >= 0x20 && code <= 0x7A) {  // 0x20 - 0x7A (except 0x40, 0x24, 0x5B-0x5F and 0x60)
      out += input[i];
    } else if (code >= 0x7B && code <= 0x7E) {  // 0x7B - 0x7E
      char c[] = { input[i], '\0' };
      auto it = gsm0338_map_.find(c);

      if (it == gsm0338_map_.end()) {  // just in case
        out += input[i];
      } else {
        out += it->second;
      }
    } else if (code >= 0x7F) {  // UTF-8 escape sequence
      std::string s;
      if (code >= 0xC0 && code <= 0xDF) {  // Double byte UTF-8
        char c[2] = { input[i], input[++i] };
        s = std::string(c, 2);
      } else if (code >= 0xE0 && code <= 0xF0) {  // Triple byte UTF-8
        char c[3] = { input[i], input[++i], input[++i] };
        s = std::string(c, 3);
      } else {  // Quad byte UTF-8
        char c[4] = { input[i], input[++i], input[++i], input[++i] };
        s = std::string(c, 4);
      }

      auto it = gsm0338_map_.find(s);
      if (it != gsm0338_map_.end()) {
        out += it->second;
      } else {
        out += '?';
      }
      // Unprintable char: ignore
    }
  }
  return out;
}

string GsmEncoder::EncodeUtf8(const string &input) {
  string out;
  // Most UTF-8 sequences are two-byte, with ASCII chars still one-byte, so double size should suffice.
  out.reserve(input.length() * 2);
  for (unsigned int i = 0; i < input.length(); i++) {
    uint8_t code = static_cast<uint8_t>(input[i]);

    if (code != 0x24 && code >= 0x20 && code <= 0x5A) {  // Shortcut (avoid dictionary)
      out += input[i];
    } else if (code >= 0x61 && code <= 0x7A) {  // Shortcut (avoid dictionary)
      out += input[i];
    } else {
      char c[2] = { input[i], '\0' };
      string s(c);
      if (code == 0x1B) {  // GSM 03.38 escape sequence read next
        c[1] = input[++i];
        s = string(c, 2);
      }
      auto it = utf8_map_.find(s);
      if (it != utf8_map_.end()) {
        out += it->second;
      } else {
        out += input[i];
      }
    }
  }
  return out;
}
}  // namespace encoding
}  // namespace smpp
