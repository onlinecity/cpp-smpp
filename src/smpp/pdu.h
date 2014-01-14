/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_PDU_H_
#define SMPP_PDU_H_

#include <stdint.h>
#include <netinet/in.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_array.hpp>

#include <iomanip>
#include <string>
#include <sstream>

#include "smpp/smpp.h"
#include "smpp/tlv.h"
#include "smpp/exceptions.h"
#include "smpp/hexdump.h"

namespace smpp {
const int HEADERFIELD_SIZE = 4;
const int HEADER_SIZE = HEADERFIELD_SIZE * 4;

/**
 * Class for representing a PDU.
 */
class PDU {
  private:
    std::stringbuf sb;
    std::iostream buf;
    uint32_t cmdId;
    uint32_t cmdStatus;
    uint32_t seqNo;
    bool nullTerminateOctetStrings;

  public:
    bool null;

    /**
     * Construct an empty PDU, ie. a null PDU
     */
    PDU();

    /**
     * Construct a PDU from a command set, useful for sending PDUs
     * @param _cmdId
     * @param _cmdStatus
     * @param _seqNo
     */
    PDU(const uint32_t &_cmdId, const uint32_t &_cmdStatus, const uint32_t &_seqNo);

    /**
     * Construct a PDU from binary data, useful for receiving PDUs
     * @param pduLength
     * @param pduBuffer
     */
    PDU(const boost::shared_array<uint8_t> &pduLength, const boost::shared_array<uint8_t> &pduBuffer);

    /**
     * Copy constructor
     * @param rhs
     */
    PDU(const PDU &rhs);

    /**
     * @return All data in this PDU as array of unsigned char array.
     */
    const boost::shared_array<uint8_t> getOctets();

    /**
     * @return PDU size in octets.
     */
    int getSize();

    /**
     * @return PDU command id.
     */
    uint32_t getCommandId() const;

    /**
     * @return PDU command status.
     */
    uint32_t getCommandStatus() const;

    /**
     * @return PDU sequence number.
     */
    uint32_t getSequenceNo() const;

    /**
     * @return True if null termination is on.
     */
    bool isNullTerminating() const;

    /**
     * Turns null termination on or off.
     * @param True if null termination is on.
     */
    void setNullTerminateOctetStrings(const bool &);

    /** Adds an integer as an unsigned 8 bit. */
    PDU &operator<<(const int &);
    PDU &operator<<(const uint8_t &i);
    PDU &operator<<(const uint16_t &i);
    PDU &operator<<(const uint32_t &i);
    PDU &operator<<(const std::basic_string<char> &s);

    PDU &operator<<(const smpp::SmppAddress);
    PDU &operator<<(const smpp::TLV);
    PDU &addOctets(const boost::shared_array<uint8_t> &octets, const std::streamsize &len);

    /**
     * Skips n octets.
     * @param n Octets to skip.
     */
    void skip(int n);

    /**
     * Resets the read marker to the beginning of the PDU.
     */
    void resetMarker();

    PDU &operator>>(int &);
    PDU &operator>>(uint8_t &i);
    PDU &operator>>(uint16_t &i);
    PDU &operator>>(uint32_t &i);
    PDU &operator>>(std::basic_string<char> &s);

    /**
     * Copy n octet into an array.
     * @param array Target array.
     * @param n Octets to copy.
     */
    void readOctets(boost::shared_array<uint8_t> &octets, const std::streamsize &n);

    /**
     * @return True if the read marker is not at the end of the PDU.
     */
    bool hasMoreData();

    static uint32_t getPduLength(boost::shared_array<uint8_t> pduHeader);
};
// PDU
std::ostream &operator<<(std::ostream &, smpp::PDU &);
}  // namespace smpp

#endif  // SMPP_PDU_H_
