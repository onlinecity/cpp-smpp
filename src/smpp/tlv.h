/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_TLV_H_
#define SMPP_TLV_H_

#include <boost/shared_array.hpp>
#include <iomanip>
#include <algorithm>
#include <string>

namespace smpp {
namespace tags {
const uint16_t DEST_ADDR_SUBUNIT = 0x0005;
const uint16_t DEST_NETWORK_TYPE = 0x0006;
const uint16_t DEST_BEARER_TYPE = 0x0007;
const uint16_t DEST_TELEMATICS_ID = 0x0008;
const uint16_t SOURCE_ADDR_SUBUNIT = 0x000D;
const uint16_t SOURCE_NETWORK_TYPE = 0x000E;
const uint16_t SOURCE_BEARER_TYPE = 0x000F;
const uint16_t SOURCE_TELEMATICS_ID = 0x0010;
const uint16_t QOS_TIME_TO_LIVE = 0x0017;
const uint16_t PAYLOAD_TYPE = 0x0019;
const uint16_t ADDITIONAL_STATUS_INFO_TEXT = 0x001D;
const uint16_t RECEIPTED_MESSAGE_ID = 0x001E;
const uint16_t MS_MSG_WAIT_FACILITIES = 0x0030;
const uint16_t PRIVACY_INDICATOR = 0x0201;
const uint16_t SOURCE_SUBADDRESS = 0x0202;
const uint16_t DEST_SUBADDRESS = 0x0203;
const uint16_t USER_MESSAGE_REFERENCE = 0x0204;
const uint16_t USER_RESPONSE_CODE = 0x0205;
const uint16_t SOURCE_PORT = 0x020A;
const uint16_t DESTINATION_PORT = 0x020B;
const uint16_t SAR_MSG_REF_NUM = 0x020C;
const uint16_t LANGUAGE_INDICATOR = 0x020D;
const uint16_t SAR_TOTAL_SEGMENTS = 0x020E;
const uint16_t SAR_SEGMENT_SEQNUM = 0x020F;
const uint16_t SC_INTERFACE_VERSION = 0x0210;
const uint16_t CALLBACK_NUM_PRES_IND = 0x0302;
const uint16_t CALLBACK_NUM_ATAG = 0x0303;
const uint16_t NUMBER_OF_MESSAGES = 0x0304;
const uint16_t CALLBACK_NUM = 0x0381;
const uint16_t DPF_RESULT = 0x0420;
const uint16_t SET_DPF = 0x0421;
const uint16_t MS_AVAILABILITY_STATUS = 0x0422;
const uint16_t NETWORK_ERROR_CODE = 0x0423;
const uint16_t MESSAGE_PAYLOAD = 0x0424;
const uint16_t DELIVERY_FAILURE_REASON = 0x0425;
const uint16_t MORE_MESSAGES_TO_SEND = 0x0426;
const uint16_t MESSAGE_STATE = 0x0427;
const uint16_t USSD_SERVICE_OP = 0x0501;
const uint16_t DISPLAY_TIME = 0x1201;
const uint16_t SMS_SIGNAL = 0x1203;
const uint16_t MS_VALIDITY = 0x1204;
const uint16_t ALERT_ON_MESSAGE_DELIVERY = 0x130C;
const uint16_t ITS_REPLY_TYPE = 0x1380;
const uint16_t ITS_SESSION_INFO = 0x1383;
}  // namespace tags

/**
 * TLV container class.
 */
class TLV {
  private:
    uint16_t tag;
    uint16_t len;
    boost::shared_array<uint8_t> octets;

  public:
    /**
     * Constructs a TLV with only the tag.
     * @param _tag TLV tag.
     */
    explicit TLV(const uint16_t &_tag) :
        tag(_tag), len(0), octets() {
    }

    /**
     * Constructs a TLV that stores a integer as an uint8_t.
     * @param _tag TLV tag.
     * @param value TLV value.
     */
    TLV(const uint16_t &_tag, int value) :
        tag(_tag), len(1), octets(new uint8_t[len]) {
        octets[0] = value & 0xff;
    }

    /**
     * Constructs a TLV with a uint8_t value.
     * @param _tag TLV tag.
     * @param value TLV value.
     */
    TLV(const uint16_t &_tag, uint8_t value) :
        tag(_tag), len(1), octets(new uint8_t[len]) {
        octets[0] = value & 0xff;
    }

    /**
     * Constructs a TLV with a uint16_t value.
     * @param _tag TLV tag.
     * @param value TLV value.
     */
    TLV(const uint16_t &_tag, uint16_t value) :
        tag(_tag), len(2), octets(new uint8_t[len]) {
        octets[0] = (value >> 8) & 0xff;
        octets[1] = value & 0xff;
    }

    /**
     * Constructs a TLV with a uint32_t value.
     * @param _tag TLV tag.
     * @param value TLV value.
     */
    TLV(const uint16_t &_tag, uint32_t value) :
        tag(_tag), len(4), octets(new uint8_t[len]) {
        octets[0] = (value >> 24) & 0xff;
        octets[1] = (value >> 16) & 0xff;
        octets[2] = (value >> 8) & 0xff;
        octets[3] = value & 0xff;
    }

    /**
     * Constructs a TLV with a string value.
     * @param _tag TLV tag.
     * @param value TLV value.
     */
    TLV(const uint16_t &_tag, std::basic_string<char> s) :
        tag(_tag), len(s.length()), octets(new uint8_t[len]) {
        std::copy(s.begin(), s.end(), octets.get());
    }

    /**
     * Constructs a TLV with an array of octets.
     * @param _tag TLV tag.
     * @param _len Length of octet array.
     * @param _octets Array of octets.
     */
    TLV(const uint16_t &_tag, const uint16_t &_len, const boost::shared_array<uint8_t> &_octets) :
        tag(_tag), len(_len), octets(_octets) {
    }

    uint16_t getTag() const {
        return tag;
    }

    uint16_t getLen() const {
        return len;
    }

    boost::shared_array<uint8_t> getOctets() const {
        return octets;
    }
};

}  // namespace smpp

#endif  // SMPP_TLV_H_
