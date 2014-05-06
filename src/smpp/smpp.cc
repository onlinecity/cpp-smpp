// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#include "smpp/smpp.h"
#include <string>

using std::string;

namespace smpp {
string GetEsmeStatus(const ESME &i) {
  switch (i) {
    case ESME::ROK:
    return "No Error";

  case ESME::RINVMSGLEN:
    return "Message Length is invalid";

  case ESME::RINVCMDLEN:
    return "Command Length is invalid";

  case ESME::RINVCMDID:
    return "Invalid Command ID";

  case ESME::RINVBNDSTS:
    return "Incorrect BIND Status for given command";

  case ESME::RALYBND:
    return "ESME Already in Bound State";

  case ESME::RINVPRTFLG:
    return "Invalid Priority Flag";

  case ESME::RINVREGDLVFLG:
    return "Invalid Registered Delivery Flag";

  case ESME::RSYSERR:
    return "System Error";

  case ESME::RINVSRCADR:
    return "Invalid Source Address";

  case ESME::RINVDSTADR:
    return "Invalid Dest Addr";

  case ESME::RINVMSGID:
    return "Message ID is invalid";

  case ESME::RBINDFAIL:
    return "Bind Failed";

  case ESME::RINVPASWD:
    return "Invalid Password";

  case ESME::RINVSYSID:
    return "Invalid System ID";

  case ESME::RCANCELFAIL:
    return "Cancel SM Failed";

  case ESME::RREPLACEFAIL:
    return "Replace SM Failed";

  case ESME::RMSGQFUL:
    return "Message Queue Full";

  case ESME::RINVSERTYP:
    return "Invalid Service Type";

  case ESME::RINVNUMDESTS:
    return "Invalid number of destinations";

  case ESME::RINVDLNAME:
    return "Invalid Distribution List name";

  case ESME::RINVDESTFLAG:
    return "Destination flag (submit_multi)";

  case ESME::RINVSUBREP:
    return "Invalid ‘submit with replace’ request (i.e. submit_sm with replace_if_present_flag set)";

  case ESME::RINVESMSUBMIT:
    return "Invalid esm_SUBMIT field data";

  case ESME::RCNTSUBDL:
    return "Cannot Submit to Distribution List";

  case ESME::RSUBMITFAIL:
    return "submit_sm or submit_multi failed";

  case ESME::RINVSRCTON:
    return "Invalid Source address TON";

  case ESME::RINVSRCNPI:
    return "Invalid Source address NPI";

  case ESME::RINVDSTTON:
    return "Invalid Destination address TON";

  case ESME::RINVDSTNPI:
    return "Invalid Destination address NPI";

  case ESME::RINVSYSTYP:
    return "Invalid system_type field";

  case ESME::RINVREPFLAG:
    return "Invalid replace_if_present flag";

  case ESME::RINVNUMMSGS:
    return "Invalid number of messages";

  case ESME::RTHROTTLED:
    return "Throttling error (ESME has exceeded allowed message limits)";

  case ESME::RINVSCHED:
    return "Invalid Scheduled Delivery Time";

  case ESME::RINVEXPIRY:
    return "Invalid message (Expiry time)";

  case ESME::RINVDFTMSGID:
    return "Predefined Message Invalid or Not Found";

  case ESME::RX_T_APPN:
    return "ESME Receiver Temporary App Error Code";

  case ESME::RX_P_APPN:
    return "ESME Receiver Permanent App Error Code";

  case ESME::RX_R_APPN:
    return "ESME Receiver Reject Message Error Code";

  case ESME::RQUERYFAIL:
    return "query_sm request failed";

  case ESME::RINVOPTPARSTREAM:
    return "Error in the optional part of the PDU Body.";

  case ESME::ROPTPARNOTALLWD:
    return "Optional Parameter not allowed";

  case ESME::RINVPARLEN:
    return "Invalid Parameter Length.";

  case ESME::RMISSINGOPTPARAM:
    return "Expected Optional Parameter missing";

  case ESME::RINVOPTPARAMVAL:
    return "Invalid Optional Parameter Value";

  case ESME::RDELIVERYFAILURE:
    return "Delivery Failure (data_sm_resp)";

  case ESME::RUNKNOWNERR:
    return "Unknown Error";

  default:
    return "Unknown";
  }
}
}  // namespace smpp
