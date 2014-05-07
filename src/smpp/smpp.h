// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#pragma once

#include <cstdint>
#include <string>

namespace smpp {
// SMPP Command ID values (table 5-1, 5.1.2.1)
enum class CommandId : uint32_t {
  NOT_DEFINED           = 0x00000000,
  GENERIC_NACK          = 0x80000000,
  BIND_RECEIVER         = 0x00000001,
  BIND_RECEIVER_RESP    = BIND_RECEIVER | GENERIC_NACK,
  BIND_TRANSMITTER      = 0x00000002,
  BIND_TRANSMITTER_RESP = BIND_TRANSMITTER | GENERIC_NACK,
  QUERY_SM              = 0x00000003,
  QUERY_SM_RESP         = QUERY_SM | GENERIC_NACK,
  SUBMIT_SM             = 0x00000004,
  SUBMIT_SM_RESP        = SUBMIT_SM | GENERIC_NACK,
  DELIVER_SM            = 0x00000005,
  DELIVER_SM_RESP       = DELIVER_SM | GENERIC_NACK,
  UNBIND                = 0x00000006,
  UNBIND_RESP           = UNBIND | GENERIC_NACK,
  REPLACE_SM            = 0x00000007,
  REPLACE_SM_RESP       = REPLACE_SM | GENERIC_NACK,
  CANCEL_SM             = 0x00000008,
  CANCEL_SM_RESP        = CANCEL_SM | GENERIC_NACK,
  BIND_TRANSCEIVER      = 0x00000009,
  BIND_TRANSCEIVER_RESP = BIND_TRANSCEIVER | GENERIC_NACK,
  OUTBIND               = 0x0000000B,
  ENQUIRE_LINK          = 0x00000015,
  ENQUIRE_LINK_RESP     = ENQUIRE_LINK | GENERIC_NACK,
  SUBMIT_MULTI          = 0x00000021,
  SUBMIT_MULTI_RESP     = SUBMIT_MULTI | GENERIC_NACK,
  ALERT_NOTIFICATION    = 0x00000102,
  DATA_SM               = 0x00000103,
  DATA_SM_RESP          = DATA_SM | GENERIC_NACK
};

// Allow construction of command respones based on the input
inline std::underlying_type<CommandId>::type operator|(const CommandId &a, const CommandId &b) {
  auto a_t = static_cast<std::underlying_type<CommandId>::type>(a);
  auto b_t = static_cast<std::underlying_type<CommandId>::type>(b);
  return a_t | b_t;
}

enum class ESME : uint32_t {
  ROK               = 0x00000000,  // No Error
  RINVMSGLEN        = 0x00000001,  // Message Length is invalid
  RINVCMDLEN        = 0x00000002,  // Command Length is invalid
  RINVCMDID         = 0x00000003,  // Invalid Command ID
  RINVBNDSTS        = 0x00000004,  // Incorrect BIND Status for given command
  RALYBND           = 0x00000005,  // ESME Already in Bound State
  RINVPRTFLG        = 0x00000006,  // Invalid Priority Flag
  RINVREGDLVFLG     = 0x00000007,  // Invalid Registered Delivery Flag
  RSYSERR           = 0x00000008,  // System Error
  RINVSRCADR        = 0x0000000A,  // Invalid Source Address
  RINVDSTADR        = 0x0000000B,  // Invalid Dest Addr
  RINVMSGID         = 0x0000000C,  // Message ID is invalid
  RBINDFAIL         = 0x0000000D,  // Bind Failed
  RINVPASWD         = 0x0000000E,  // Invalid Password
  RINVSYSID         = 0x0000000F,  // Invalid System ID
  RCANCELFAIL       = 0x00000011,  // Cancel SM Failed
  RREPLACEFAIL      = 0x00000013,  // Replace SM Failed
  RMSGQFUL          = 0x00000014,  // Message Queue Full
  RINVSERTYP        = 0x00000015,  // Invalid Service Type
  RINVNUMDESTS      = 0x00000033,  // Invalid number of destinations
  RINVDLNAME        = 0x00000034,  // Invalid Distribution List name
  RINVDESTFLAG      = 0x00000040,  // Destination flag (submit_multi)
  RINVSUBREP        = 0x00000042,  // Invalid ‘submit with replace’ request
                                   // (i.e. submit_sm with
                                   // replace_if_present_flag set)
  RINVESMSUBMIT     = 0x00000043,  // Invalid esm_SUBMIT field data
  RCNTSUBDL         = 0x00000044,  // Cannot Submit to Distribution List
  RSUBMITFAIL       = 0x00000045,  // submit_sm or submit_multi failed
  RINVSRCTON        = 0x00000048,  // Invalid Source address TON
  RINVSRCNPI        = 0x00000049,  // Invalid Source address NPI
  RINVDSTTON        = 0x00000050,  // Invalid Destination address TON
  RINVDSTNPI        = 0x00000051,  // Invalid Destination address NPI
  RINVSYSTYP        = 0x00000053,  // Invalid system_type field
  RINVREPFLAG       = 0x00000054,  // Invalid replace_if_present flag
  RINVNUMMSGS       = 0x00000055,  // Invalid number of messages
  RTHROTTLED        = 0x00000058,  // Throttling error (ESME has exceeded
                                   // allowed message limits)
  RINVSCHED         = 0x00000061,  // Invalid Scheduled Delivery Time
  RINVEXPIRY        = 0x00000062,  // Invalid message (Expiry time)
  RINVDFTMSGID      = 0x00000063,  // Predefined Message Invalid or Not Found
  RX_T_APPN         = 0x00000064,  // ESME Receiver Temporary App Error Code
  RX_P_APPN         = 0x00000065,  // ESME Receiver Permanent App Error Code
  RX_R_APPN         = 0x00000066,  // ESME Receiver Reject Message Error Code
  RQUERYFAIL        = 0x00000067,  // query_sm request failed
  RINVOPTPARSTREAM  = 0x000000C0,  // Error in the optional part of the PDU Body.
  ROPTPARNOTALLWD   = 0x000000C1,  // Optional Parameter not allowed
  RINVPARLEN        = 0x000000C2,  // Invalid Parameter Length.
  RMISSINGOPTPARAM  = 0x000000C3,  // Expected Optional Parameter missing
  RINVOPTPARAMVAL   = 0x000000C4,  // Invalid Optional Parameter Value
  RDELIVERYFAILURE  = 0x000000FE,  // Delivery Failure (data_sm_resp)
  RUNKNOWNERR       = 0x000000FF  // Unknown Error
};

enum class InterfaceVersion : uint8_t {
  SMPP_3_4  = 0x34
};

// SMPP v3.4 - 5.2.5 page 117
enum class TON : uint8_t {
  UNKNOWN           = 0x00,
  INTERNATIONAL     = 0x01,
  NATIONAL          = 0x02,
  NETWORKSPECIFIC   = 0x03,
  SUBSCRIBERNUMBER  = 0x04,
  ALPHANUMERIC      = 0x05,
  ABBREVIATED       = 0x06
};

// SMPP v3.4 - 5.2.6 page 118
enum class NPI : uint8_t {
  UNKNOWN   = 0x00,
  E164      = 0x01,
  DATA      = 0x03,
  TELEX     = 0x04,
  E212      = 0x06,
  NATIONAL  = 0x08,
  PRIVATE   = 0x09,
  ERMES     = 0x0a,
  INTERNET  = 0x0e,
  WAPCLIENT = 0x12
};

enum class ESM : uint8_t {
  // ESM bits 1-0 - SMPP v3.4 - 5.2.12 page 121-122
  SUBMIT_MODE_SMSC_DEFAULT    = 0x00,
  SUBMIT_MODE_DATAGRAM        = 0x01,
  SUBMIT_MODE_FORWARD         = 0x02,
  SUBMIT_MODE_STOREANDFORWARD = 0x03,
  // ESM bits 5-2
  SUBMIT_BINARY               = 0x04,
  SUBMIT_TYPE_ESME_D_ACK      = 0x08,
  SUBMIT_TYPE_ESME_U_ACK      = 0x10,
  DELIVER_SMSC_RECEIPT        = 0x04,
  DELIVER_SME_ACK             = 0x08,
  DELIVER_U_ACK               = 0x10,
  DELIVER_CONV_ABORT          = 0x18,
  DELIVER_IDN                 = 0x20,  // Intermediate delivery notification
  // ESM bits 7-6
  UHDI                        = 0x40,
  REPLYPATH                   = 0x80
};

// Allow combinations of ESM to be piped to PDU
inline std::underlying_type<ESM>::type operator|(const ESM &a, const ESM &b) {
  auto a_t = static_cast<std::underlying_type<ESM>::type>(a);
  auto b_t = static_cast<std::underlying_type<ESM>::type>(b);
  return a_t | b_t;
}

// SMPP v3.4 - 5.2.14 page 123 - priority_flag
enum class Priority : uint8_t {
  GSM_NON_PRIORITY      = 0x0,
  GSM_PRIORITY          = 0x1,
  ANSI_136_BULK         = 0x0,
  ANSI_136_NORMAL       = 0x1,
  ANSI_136_URGENT       = 0x2,
  ANSI_136_VERY_URGENT  = 0x3,
  IS_95_NORMAL          = 0x0,
  IS_95_INTERACTIVE     = 0x1,
  IS_95_URGENT          = 0x2,
  IS_95_EMERGENCY       = 0x3
};

// SMPP v3.4 - 5.2.17 page 124
enum class RegisteredDelivery : uint8_t {
  DELIVERY_NO           = 0x00,
  DELIVERY_SMSC_BOTH    = 0x01,  // both success and failure
  DELIVERY_SMSC_FAILED  = 0x02,
  DELIVERY_SME_D_ACK    = 0x04,
  DELIVERY_SME_U_ACK    = 0x08,
  DELIVERY_SME_BOTH     = 0x10,
  DELIVERY_IDN          = 0x16  // Intermediate notification
};

// SMPP v3.4 - 5.2.18 page 125
enum class ReplaceIfPresent : uint8_t {
  REPLACE_NO  = 0x00,
  REPLACE_YES = 0x01
};

// SMPP v3.4 - 5.2.19 page 126
enum class DataCoding : uint8_t {
  DEFAULT       = 0,
  IA5           = 1,  // IA5 (CCITT T.50)/ASCII (ANSI X3.4)
  BINARY_ALIAS  = 2,
  ISO8859_1     = 3,  // Latin 1
  BINARY        = 4,
  JIS           = 5,
  ISO8859_5     = 6,  // Cyrllic
  ISO8859_8     = 7,  // Latin/Hebrew
  UCS2          = 8,  // UCS-2BE (Big Endian)
  PICTOGRAM     = 9,
  ISO2022_JP    = 10,  // Music codes
  KANJI         = 13,  // Extended Kanji JIS
  KSC5601       = 14
};

// SMPP v3.4 - 5.2.25 page 129
enum class DestFlag : uint8_t {
  DEST_FLAG_SME       = 1,
  DEST_FLAG_DISTLIST  = 2
};

// SMPP v3.4 - 5.2.28 page 130
enum class State : uint8_t {
  ENROUTE       = 1,
  DELIVERED     = 2,
  EXPIRED       = 3,
  DELETED       = 4,
  UNDELIVERABLE = 5,
  ACCEPTED      = 6,
  UNKNOWN       = 7,
  REJECTED      = 8
};

std::string GetEsmeStatus(const ESME &);

template<class Enum, class = typename std::enable_if<std::is_enum<Enum>::value>::type>
std::ostream &operator<<(std::ostream &out, Enum &e) {
  out << static_cast<typename std::underlying_type<Enum>::type >(e);
  return out;
}

struct SmppAddress {
  std::string value;
  smpp::TON ton;  // type-of-number
  smpp::NPI npi;  // numbering-plan-indicator

  explicit SmppAddress(const std::string &val) :
    value(val),
    ton(smpp::TON::UNKNOWN),
    npi(smpp::NPI::UNKNOWN) {
  }

  SmppAddress(const std::string &val,
              const smpp::TON type_of_number,
              const smpp::NPI numbering_plan_indicator) :
    value(val),
    ton(type_of_number),
    npi(numbering_plan_indicator) {
  }
};
}  // namespace smpp
