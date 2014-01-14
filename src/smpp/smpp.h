/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_SMPP_H_
#define SMPP_SMPP_H_

#include <stdint.h>
#include <string>

namespace smpp {
/*
 * SMPP Command ID values (table 5-1, 5.1.2.1)
 */
const uint32_t GENERIC_NACK = 0x80000000;
const uint32_t BIND_RECEIVER = 0x00000001;
const uint32_t BIND_RECEIVER_RESP = 0x80000001;
const uint32_t BIND_TRANSMITTER = 0x00000002;
const uint32_t BIND_TRANSMITTER_RESP = 0x80000002;
const uint32_t QUERY_SM = 0x00000003;
const uint32_t QUERY_SM_RESP = 0x80000003;
const uint32_t SUBMIT_SM = 0x00000004;
const uint32_t SUBMIT_SM_RESP = 0x80000004;
const uint32_t DELIVER_SM = 0x00000005;
const uint32_t DELIVER_SM_RESP = 0x80000005;
const uint32_t UNBIND = 0x00000006;
const uint32_t UNBIND_RESP = 0x80000006;
const uint32_t REPLACE_SM = 0x00000007;
const uint32_t REPLACE_SM_RESP = 0x80000007;
const uint32_t CANCEL_SM = 0x00000008;
const uint32_t CANCEL_SM_RESP = 0x80000008;
const uint32_t BIND_TRANSCEIVER = 0x00000009;
const uint32_t BIND_TRANSCEIVER_RESP = 0x80000009;
const uint32_t OUTBIND = 0x0000000B;
const uint32_t ENQUIRE_LINK = 0x00000015;
const uint32_t ENQUIRE_LINK_RESP = 0x80000015;
const uint32_t SUBMIT_MULTI = 0x00000021;
const uint32_t SUBMIT_MULTI_RESP = 0x80000021;
const uint32_t ALERT_NOTIFICATION = 0x00000102;
const uint32_t RESERVED = 0x80000102;
const uint32_t DATA_SM = 0x00000103;
const uint32_t DATA_SM_RESP = 0x80000103;

const uint32_t ESME_ROK = 0x00000000;  // No Error
const uint32_t ESME_RINVMSGLEN = 0x00000001;  // Message Length is invalid
const uint32_t ESME_RINVCMDLEN = 0x00000002;  // Command Length is invalid
const uint32_t ESME_RINVCMDID = 0x00000003;  // Invalid Command ID
const uint32_t ESME_RINVBNDSTS = 0x00000004;  // Incorrect BIND Status for given command
const uint32_t ESME_RALYBND = 0x00000005;  // ESME Already in Bound State
const uint32_t ESME_RINVPRTFLG = 0x00000006;  // Invalid Priority Flag
const uint32_t ESME_RINVREGDLVFLG = 0x00000007;  // Invalid Registered Delivery Flag
const uint32_t ESME_RSYSERR = 0x00000008;  // System Error
const uint32_t ESME_RINVSRCADR = 0x0000000A;  // Invalid Source Address
const uint32_t ESME_RINVDSTADR = 0x0000000B;  // Invalid Dest Addr
const uint32_t ESME_RINVMSGID = 0x0000000C;  // Message ID is invalid
const uint32_t ESME_RBINDFAIL = 0x0000000D;  // Bind Failed
const uint32_t ESME_RINVPASWD = 0x0000000E;  // Invalid Password
const uint32_t ESME_RINVSYSID = 0x0000000F;  // Invalid System ID
const uint32_t ESME_RCANCELFAIL = 0x00000011;  // Cancel SM Failed
const uint32_t ESME_RREPLACEFAIL = 0x00000013;  // Replace SM Failed
const uint32_t ESME_RMSGQFUL = 0x00000014;  // Message Queue Full
const uint32_t ESME_RINVSERTYP = 0x00000015;  // Invalid Service Type
const uint32_t ESME_RINVNUMDESTS = 0x00000033;  // Invalid number of destinations
const uint32_t ESME_RINVDLNAME = 0x00000034;  // Invalid Distribution List name
const uint32_t ESME_RINVDESTFLAG = 0x00000040;  // Destination flag (submit_multi)
const uint32_t ESME_RINVSUBREP =
    0x00000042;  // Invalid ‘submit with replace’ request (i.e. submit_sm with replace_if_present_flag set)
const uint32_t ESME_RINVESMSUBMIT = 0x00000043;  // Invalid esm_SUBMIT field data
const uint32_t ESME_RCNTSUBDL = 0x00000044;  // Cannot Submit to Distribution List
const uint32_t ESME_RSUBMITFAIL = 0x00000045;  // submit_sm or submit_multi failed
const uint32_t ESME_RINVSRCTON = 0x00000048;  // Invalid Source address TON
const uint32_t ESME_RINVSRCNPI = 0x00000049;  // Invalid Source address NPI
const uint32_t ESME_RINVDSTTON = 0x00000050;  // Invalid Destination address TON
const uint32_t ESME_RINVDSTNPI = 0x00000051;  // Invalid Destination address NPI
const uint32_t ESME_RINVSYSTYP = 0x00000053;  // Invalid system_type field
const uint32_t ESME_RINVREPFLAG = 0x00000054;  // Invalid replace_if_present flag
const uint32_t ESME_RINVNUMMSGS = 0x00000055;  // Invalid number of messages
const uint32_t ESME_RTHROTTLED = 0x00000058;  // Throttling error (ESME has exceeded allowed message limits)
const uint32_t ESME_RINVSCHED = 0x00000061;  // Invalid Scheduled Delivery Time
const uint32_t ESME_RINVEXPIRY = 0x00000062;  // Invalid message (Expiry time)
const uint32_t ESME_RINVDFTMSGID = 0x00000063;  // Predefined Message Invalid or Not Found
const uint32_t ESME_RX_T_APPN = 0x00000064;  // ESME Receiver Temporary App Error Code
const uint32_t ESME_RX_P_APPN = 0x00000065;  // ESME Receiver Permanent App Error Code
const uint32_t ESME_RX_R_APPN = 0x00000066;  // ESME Receiver Reject Message Error Code
const uint32_t ESME_RQUERYFAIL = 0x00000067;  // query_sm request failed
const uint32_t ESME_RINVOPTPARSTREAM = 0x000000C0;  // Error in the optional part of the PDU Body.
const uint32_t ESME_ROPTPARNOTALLWD = 0x000000C1;  // Optional Parameter not allowed
const uint32_t ESME_RINVPARLEN = 0x000000C2;  // Invalid Parameter Length.
const uint32_t ESME_RMISSINGOPTPARAM = 0x000000C3;  // Expected Optional Parameter missing
const uint32_t ESME_RINVOPTPARAMVAL = 0x000000C4;  // Invalid Optional Parameter Value
const uint32_t ESME_RDELIVERYFAILURE = 0x000000FE;  // Delivery Failure (data_sm_resp)
const uint32_t ESME_RUNKNOWNERR = 0x000000FF;  // Unknown Error

// SMPP v3.4 - 5.2.5 page 117
const uint8_t TON_UNKNOWN = 0x00;
const uint8_t TON_INTERNATIONAL = 0x01;
const uint8_t TON_NATIONAL = 0x02;
const uint8_t TON_NETWORKSPECIFIC = 0x03;
const uint8_t TON_SUBSCRIBERNUMBER = 0x04;
const uint8_t TON_ALPHANUMERIC = 0x05;
const uint8_t TON_ABBREVIATED = 0x06;

// SMPP v3.4 - 5.2.6 page 118
const uint8_t NPI_UNKNOWN = 0x00;
const uint8_t NPI_E164 = 0x01;
const uint8_t NPI_DATA = 0x03;
const uint8_t NPI_TELEX = 0x04;
const uint8_t NPI_E212 = 0x06;
const uint8_t NPI_NATIONAL = 0x08;
const uint8_t NPI_PRIVATE = 0x09;
const uint8_t NPI_ERMES = 0x0a;
const uint8_t NPI_INTERNET = 0x0e;
const uint8_t NPI_WAPCLIENT = 0x12;

// ESM bits 1-0 - SMPP v3.4 - 5.2.12 page 121-122
const uint8_t ESM_SUBMIT_MODE_SMSC_DEFAULT = 0x00;
const uint8_t ESM_SUBMIT_MODE_DATAGRAM = 0x01;
const uint8_t ESM_SUBMIT_MODE_FORWARD = 0x02;
const uint8_t ESM_SUBMIT_MODE_STOREANDFORWARD = 0x03;
// ESM bits 5-2
const uint8_t ESM_SUBMIT_BINARY = 0x04;
const uint8_t ESM_SUBMIT_TYPE_ESME_D_ACK = 0x08;
const uint8_t ESM_SUBMIT_TYPE_ESME_U_ACK = 0x10;
const uint8_t ESM_DELIVER_SMSC_RECEIPT = 0x04;
const uint8_t ESM_DELIVER_SME_ACK = 0x08;
const uint8_t ESM_DELIVER_U_ACK = 0x10;
const uint8_t ESM_DELIVER_CONV_ABORT = 0x18;
const uint8_t ESM_DELIVER_IDN = 0x20;  // Intermediate delivery notification
// ESM bits 7-6
const uint8_t ESM_UHDI = 0x40;
const uint8_t ESM_REPLYPATH = 0x80;

// SMPP v3.4 - 5.2.14 page 123 - priority_flag
const uint8_t PRIORITY_GSM_NON_PRIORITY = 0x0;
const uint8_t PRIORITY_GSM_PRIORITY = 0x1;

const uint8_t PRIORITY_ANSI_136_BULK = 0x0;
const uint8_t PRIORITY_ANSI_136_NORMAL = 0x1;
const uint8_t PRIORITY_ANSI_136_URGENT = 0x2;
const uint8_t PRIORITY_ANSI_136_VERY_URGENT = 0x3;

const uint8_t PRIORITY_IS_95_NORMAL = 0x0;
const uint8_t PRIORITY_IS_95_INTERACTIVE = 0x1;
const uint8_t PRIORITY_IS_95_URGENT = 0x2;
const uint8_t PRIORITY_IS_95_EMERGENCY = 0x3;

// SMPP v3.4 - 5.2.17 page 124
const uint8_t REG_DELIVERY_NO = 0x00;
const uint8_t REG_DELIVERY_SMSC_BOTH = 0x01;  // both success and failure
const uint8_t REG_DELIVERY_SMSC_FAILED = 0x02;
const uint8_t REG_DELIVERY_SME_D_ACK = 0x04;
const uint8_t REG_DELIVERY_SME_U_ACK = 0x08;
const uint8_t REG_DELIVERY_SME_BOTH = 0x10;
const uint8_t REG_DELIVERY_IDN = 0x16;  // Intermediate notification

// SMPP v3.4 - 5.2.18 page 125
const uint8_t REPLACE_NO = 0x00;
const uint8_t REPLACE_YES = 0x01;

// SMPP v3.4 - 5.2.19 page 126
const uint8_t DATA_CODING_DEFAULT = 0;
const uint8_t DATA_CODING_IA5 = 1;  // IA5 (CCITT T.50)/ASCII (ANSI X3.4)
const uint8_t DATA_CODING_BINARY_ALIAS = 2;
const uint8_t DATA_CODING_ISO8859_1 = 3;  // Latin 1
const uint8_t DATA_CODING_BINARY = 4;
const uint8_t DATA_CODING_JIS = 5;
const uint8_t DATA_CODING_ISO8859_5 = 6;  // Cyrllic
const uint8_t DATA_CODING_ISO8859_8 = 7;  // Latin/Hebrew
const uint8_t DATA_CODING_UCS2 = 8;  // UCS-2BE (Big Endian)
const uint8_t DATA_CODING_PICTOGRAM = 9;
const uint8_t DATA_CODING_ISO2022_JP = 10;  // Music codes
const uint8_t DATA_CODING_KANJI = 13;  // Extended Kanji JIS
const uint8_t DATA_CODING_KSC5601 = 14;

// SMPP v3.4 - 5.2.25 page 129
const uint8_t DEST_FLAG_SME = 1;
const uint8_t DEST_FLAG_DISTLIST = 2;

// SMPP v3.4 - 5.2.28 page 130
const uint8_t STATE_ENROUTE = 1;
const uint8_t STATE_DELIVERED = 2;
const uint8_t STATE_EXPIRED = 3;
const uint8_t STATE_DELETED = 4;
const uint8_t STATE_UNDELIVERABLE = 5;
const uint8_t STATE_ACCEPTED = 6;
const uint8_t STATE_UNKNOWN = 7;
const uint8_t STATE_REJECTED = 8;

std::string getEsmeStatus(uint32_t);

class SmppAddress {
  public:
    std::string value;
    uint8_t ton;  // type-of-number
    uint8_t npi;  // numbering-plan-indicator

    SmppAddress(const std::string &_value, const uint8_t &_ton = smpp::TON_UNKNOWN, const uint8_t &_npi =
                    smpp::NPI_UNKNOWN) :
        value(_value), ton(_ton), npi(_npi) {
    }
};
}  // namespace smpp
#endif  // SMPP_SMPP_H_
