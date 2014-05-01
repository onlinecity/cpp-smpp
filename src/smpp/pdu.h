// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#pragma once

#include <string>
#include <sstream>
#include <array>

#include "smpp/smpp.h"
#include "smpp/tlv.h"
#include "smpp/exceptions.h"

namespace smpp {

const int HEADERFIELD_SIZE = 4;

typedef std::string PduData;
typedef std::array<char, HEADERFIELD_SIZE> PduLengthHeader;

// Class for representing a PDU.
class PDU {
 public:
  // Construct an empty PDU, ie. a null PDU
  PDU();

  // Construct a PDU from a command set, useful for sending PDUs
  PDU(const uint32_t &_cmdId, const uint32_t &_cmdStatus, const uint32_t &_seqNo);

  // Construct a PDU from binary data, useful for receiving PDUs
  PDU(const PduLengthHeader &pduLength, const PduData &pduBuffer);

  PDU(const PDU &rhs);

  // Returns a copy of all data in this PDU as array of unsigned char array.
  const PduData GetOctets();

  // @return PDU size in octets.
  int Size();

  uint32_t command_id() const {
    return command_id_;
  }

  uint32_t command_status() const {
    return command_status_;
  }

  uint32_t sequence_no() const {
    return seq_no_;
  }

  bool null_terminate_octet_strings() const {
    return null_terminate_octet_strings_;
  }

  void set_null_terminate_octet_strings(const bool null_terminate_octet_strings) {
    null_terminate_octet_strings_ = null_terminate_octet_strings;
  }

  bool null() const {
    return null_;
  }

  // Adds an integer as an unsigned 8 bit.
  PDU &operator<<(const int &);
  PDU &operator<<(const uint8_t &i);
  PDU &operator<<(const uint16_t &i);
  PDU &operator<<(const uint32_t &i);
  PDU &operator<<(const std::basic_string<char> &s);

  PDU &operator<<(const smpp::SmppAddress);
  PDU &operator<<(const smpp::TLV);
  PDU &addOctets(const PduData &octets, const std::streamsize &len);

  // Skips n octets.
  void Skip(int n);

  // Resets the read marker to the beginning of the PDU.
  void ResetMarker();

  PDU &operator>>(int &);
  PDU &operator>>(uint8_t &i);
  PDU &operator>>(uint16_t &i);
  PDU &operator>>(uint32_t &i);
  PDU &operator>>(std::basic_string<char> &s);

  // Copy n octet into an array.
  // @param array Target array.
  // @param n Octets to copy.
  void ReadOctets(PduData *octets, const std::streamsize &n);

  bool HasMoreData();

  static uint32_t GetPduLength(const PduLengthHeader &pduHeader);

  std::stringbuf sb_;
  std::iostream buf_;
  uint32_t command_id_;
  uint32_t command_status_;
  uint32_t seq_no_;
  bool null_terminate_octet_strings_;
  bool null_;
};
std::ostream &operator<<(std::ostream &, smpp::PDU &);
}  // namespace smpp
