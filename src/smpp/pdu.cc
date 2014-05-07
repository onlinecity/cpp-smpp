// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#include "smpp/pdu.h"
#include <iostream>
#include <netinet/in.h>
#include <string>
#include "hexdump.h"

namespace smpp {
using std::ios;
using std::ios_base;
using std::ends;
using std::streamsize;
using std::dec;
using std::hex;
using std::endl;

PDU::PDU() :
  sb_(""),
  buf_(&sb_),
  command_id_(0),
  command_status_(ESME::ROK),
  seq_no_(0),
  null_terminate_octet_strings_(true),
  null_(true) {
}

PDU::PDU(
    const uint32_t &command_id,
    const ESME &command_status,
    const uint32_t &seq_no) :
  sb_(""),
  buf_(&sb_),
  command_id_(command_id),
  command_status_(command_status),
  seq_no_(seq_no),
  null_terminate_octet_strings_(true),
  null_(false) {
  (*this) << uint32_t(0);
  (*this) << command_id_;
  (*this) << command_status_;
  (*this) << seq_no_;
}

PDU::PDU(
    const PduLengthHeader &pduLength,
    const PduData &pduBuffer) :
  sb_(""),
  buf_(&sb_),
  command_id_(0),
  command_status_(ESME::ROK),
  seq_no_(0),
  null_terminate_octet_strings_(true),
  null_(false) {
  uint32_t bufSize = PDU::GetPduLength(pduLength);
  buf_.write(pduLength.data(), HEADERFIELD_SIZE);

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write length");
  }

  buf_.write(pduBuffer.c_str(), bufSize - HEADERFIELD_SIZE);

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write octets");
  }

  buf_.seekg(HEADERFIELD_SIZE, std::ios::cur);

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to skip size header");
  }

  (*this) >> command_id_;
  (*this) >> command_status_;
  (*this) >> seq_no_;
}

PDU::PDU(const PDU &rhs) :
  sb_(rhs.sb_.str()),
  buf_(&sb_),
  command_id_(rhs.command_id_),
  command_status_(rhs.command_status_),
  seq_no_(rhs.seq_no_),
  null_terminate_octet_strings_(rhs.null_terminate_octet_strings_),
  null_(rhs.null_) {
  ResetMarker();  // remember to reset the marker after copying.
}

const PduData PDU::GetOctets() {
  uint32_t size = Size();
  uint32_t beSize = htonl(size);
  buf_.seekp(0, ios::beg);
  buf_.write(reinterpret_cast<char*>(&beSize), sizeof(uint32_t));

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write length");
  }

  buf_.seekp(0, ios::end);
  buf_.seekg(0, ios::beg);
  PduData octets;
  octets.resize(size);
  buf_.read(&*octets.begin(), size);

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to read octets");
  }

  // Seek to start of PDU body
  ResetMarker();
  return octets;
}

int PDU::Size() {
  buf_.seekp(0, ios_base::end);
  int s = buf_.tellp();
  return s;
}

PDU &PDU::operator<<(const int &i) {
  uint8_t x(i);
  buf_.write(reinterpret_cast<char*>(&x), sizeof(uint8_t));

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write int");
  }

  return *this;
}

PDU &PDU::operator<<(const uint8_t &i) {
  uint8_t x(i);
  buf_.write(reinterpret_cast<char*>(&x), sizeof(uint8_t));

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write uint8_t");
  }

  return *this;
}

PDU &PDU::operator<<(const uint16_t &i) {
  uint16_t j = htons(i);
  buf_.write(reinterpret_cast<char*>(&j), sizeof(uint16_t));

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write uint16_t");
  }

  return *this;
}

PDU &PDU::operator<<(const uint32_t &i) {
  uint32_t j = htonl(i);
  buf_.write(reinterpret_cast<char*>(&j), sizeof(uint32_t));

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write uint32_t");
  }

  return *this;
}

PDU &PDU::operator<<(const std::basic_string<char> &s) {
  buf_.write(s.c_str(), s.length());  // use buf.write to allow for UCS-2 chars which are 16-bit.

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write string");
  }

  if (null_terminate_octet_strings_) {
    buf_ << ends;
  }

  return *this;
}

PDU &PDU::operator <<(const smpp::SmppAddress s) {
  (*this) << s.ton;
  (*this) << s.npi;
  (*this) << s.value;
  return *this;
}

PDU &PDU::operator <<(smpp::TLV tlv) {
  (*this) << tlv.getTag();
  (*this) << tlv.getLen();

  if (tlv.getLen() != 0) {
    (*this).addOctets(tlv.getOctets(), (uint32_t) tlv.getLen());
  }

  return *this;
}

// TODO(td): Upper case
PDU &PDU::addOctets(const PduData &octets, const streamsize &len) {
  buf_.write(octets.c_str(), len);

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to write octets");
  }

  return *this;
}

void PDU::Skip(int octets) {
  buf_.seekg(octets, ios_base::cur);

  if (buf_.fail()) {
    throw smpp::SmppException(buf_.bad() ? "Last PDU IO operation failed" : "PDU seek to invalid pos");
  }
}

void PDU::ResetMarker() {
  // Seek to start of PDU body (after headers)
  buf_.seekg(HEADERFIELD_SIZE * 4, ios::beg);

  if (buf_.fail()) {
    throw smpp::SmppException("PDU failed to reset marker");
  }
}

PDU &PDU::operator>>(int &i) {
  uint8_t j;
  buf_.read(reinterpret_cast<char*>(&j), sizeof(uint8_t));

  if (buf_.fail()) {
    throw smpp::SmppException(buf_.eof() ? "PDU reached EOF" : "Last PDU IO operation failed");
  }

  i = j;
  return *this;
}

PDU &PDU::operator>>(uint8_t &i) {
  buf_.read(reinterpret_cast<char*>(&i), sizeof(uint8_t));

  if (buf_.fail()) {
    throw smpp::SmppException(buf_.eof() ? "PDU reached EOF" : "Last PDU IO operation failed");
  }

  return *this;
}

PDU &PDU::operator>>(uint16_t &i) {
  buf_.read(reinterpret_cast<char*>(&i), sizeof(uint16_t));

  if (buf_.fail()) {
    throw smpp::SmppException(buf_.eof() ? "PDU reached EOF" : "Last PDU IO operation failed");
  }

  i = ntohs(i);
  return *this;
}

PDU &PDU::operator>>(uint32_t &i) {
  buf_.read(reinterpret_cast<char*>(&i), sizeof(uint32_t));

  if (buf_.fail()) {
    throw smpp::SmppException(buf_.eof() ? "PDU reached EOF" : "Last PDU IO operation failed");
  }

  i = ntohl(i);
  return *this;
}

PDU &PDU::operator>>(std::basic_string<char> &s) {
  getline(buf_, s, '\0');
  return *this;
}

void PDU::ReadOctets(PduData *octets, const streamsize &len) {
  octets->resize(len);
  buf_.readsome(&*octets->begin(), len);

  if (buf_.fail()) {
    throw smpp::SmppException(buf_.eof() ? "PDU reached EOF" : "Last PDU IO operation failed");
  }
}

bool PDU::HasMoreData() {
  buf_.peek();  // peek sets eof, it's not set until you try to read or peek at the data
  return !buf_.eof();
}

uint32_t PDU::GetPduLength(const PduLengthHeader &pduHeader) {
  auto i = reinterpret_cast<const uint32_t*>(pduHeader.data());
  return ntohl(*i);
}

}  // namespace smpp


std::ostream &smpp::operator<<(std::ostream &out, smpp::PDU &pdu) {
  if (pdu.null_) {
    out << "PDU IS NULL" << std::endl;
    return out;
  }

  int size = pdu.Size();
  out << "size      :" << pdu.Size() << endl << "sequence  :" << pdu.sequence_no() << endl <<
      "cmd id    :0x"
      << hex << pdu.command_id() << dec << endl << "cmd status:0x" << hex << static_cast<std::underlying_type<smpp::ESME>::type>(pdu.command_status()) <<
      dec << " : "
      << smpp::GetEsmeStatus(pdu.command_status()) << endl;
  out << oc::tools::hexdump(reinterpret_cast<const unsigned char*>(pdu.GetOctets().c_str()), static_cast<size_t>(size));
  return out;
}
