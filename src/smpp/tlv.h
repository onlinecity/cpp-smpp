// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#pragma once

#include <algorithm>
#include <iomanip>
#include <string>
#include <type_traits>

namespace smpp {

enum class Tag : uint16_t {
  NA                          = 0,
  DEST_ADDR_SUBUNIT           = 0x0005,
  DEST_NETWORK_TYPE           = 0x0006,
  DEST_BEARER_TYPE            = 0x0007,
  DEST_TELEMATICS_ID          = 0x0008,
  SOURCE_ADDR_SUBUNIT         = 0x000D,
  SOURCE_NETWORK_TYPE         = 0x000E,
  SOURCE_BEARER_TYPE          = 0x000F,
  SOURCE_TELEMATICS_ID        = 0x0010,
  QOS_TIME_TO_LIVE            = 0x0017,
  PAYLOAD_TYPE                = 0x0019,
  ADDITIONAL_STATUS_INFO_TEXT = 0x001D,
  RECEIPTED_MESSAGE_ID        = 0x001E,
  MS_MSG_WAIT_FACILITIES      = 0x0030,
  PRIVACY_INDICATOR           = 0x0201,
  SOURCE_SUBADDRESS           = 0x0202,
  DEST_SUBADDRESS             = 0x0203,
  USER_MESSAGE_REFERENCE      = 0x0204,
  USER_RESPONSE_CODE          = 0x0205,
  SOURCE_PORT                 = 0x020A,
  DESTINATION_PORT            = 0x020B,
  SAR_MSG_REF_NUM             = 0x020C,
  LANGUAGE_INDICATOR          = 0x020D,
  SAR_TOTAL_SEGMENTS          = 0x020E,
  SAR_SEGMENT_SEQNUM          = 0x020F,
  SC_INTERFACE_VERSION        = 0x0210,
  CALLBACK_NUM_PRES_IND       = 0x0302,
  CALLBACK_NUM_ATAG           = 0x0303,
  NUMBER_OF_MESSAGES          = 0x0304,
  CALLBACK_NUM                = 0x0381,
  DPF_RESULT                  = 0x0420,
  SET_DPF                     = 0x0421,
  MS_AVAILABILITY_STATUS      = 0x0422,
  NETWORK_ERROR_CODE          = 0x0423,
  MESSAGE_PAYLOAD             = 0x0424,
  DELIVERY_FAILURE_REASON     = 0x0425,
  MORE_MESSAGES_TO_SEND       = 0x0426,
  MESSAGE_STATE               = 0x0427,
  USSD_SERVICE_OP             = 0x0501,
  DISPLAY_TIME                = 0x1201,
  SMS_SIGNAL                  = 0x1203,
  MS_VALIDITY                 = 0x1204,
  ALERT_ON_MESSAGE_DELIVERY   = 0x130C,
  ITS_REPLY_TYPE              = 0x1380,
  ITS_SESSION_INFO            = 0x1383
};

// TLV container class.
class TLV {
 public:
  explicit TLV(const Tag &tag):
    tag_(tag), len_(0), octets_() {
  }

  template <class Integral, class = typename std::enable_if<std::is_integral<Integral>::value >::type>
  TLV(const Tag &tag, Integral value):
    tag_(tag), len_(sizeof(value)), octets_(std::to_string(value)) {
  }

  TLV(const Tag &tag, const std::basic_string<char> &s) :
    tag_(tag), len_(s.length()), octets_(s) {
  }

  inline Tag tag() const {
    return tag_;
  }

  inline uint16_t len() const {
    return len_;
  }

  inline std::string octets() const {
    return octets_;
  }

 private:
  Tag tag_;
  uint16_t len_;
  std::string octets_;
};
}  // namespace smpp
