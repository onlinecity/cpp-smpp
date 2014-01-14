/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "smpp/smpp.h"
#include <string>

using std::string;

namespace smpp {
string getEsmeStatus(uint32_t i) {
    switch (i) {
    case ESME_ROK:
        return "No Error";

    case ESME_RINVMSGLEN:
        return "Message Length is invalid";

    case ESME_RINVCMDLEN:
        return "Command Length is invalid";

    case ESME_RINVCMDID:
        return "Invalid Command ID";

    case ESME_RINVBNDSTS:
        return "Incorrect BIND Status for given command";

    case ESME_RALYBND:
        return "ESME Already in Bound State";

    case ESME_RINVPRTFLG:
        return "Invalid Priority Flag";

    case ESME_RINVREGDLVFLG:
        return "Invalid Registered Delivery Flag";

    case ESME_RSYSERR:
        return "System Error";

    case ESME_RINVSRCADR:
        return "Invalid Source Address";

    case ESME_RINVDSTADR:
        return "Invalid Dest Addr";

    case ESME_RINVMSGID:
        return "Message ID is invalid";

    case ESME_RBINDFAIL:
        return "Bind Failed";

    case ESME_RINVPASWD:
        return "Invalid Password";

    case ESME_RINVSYSID:
        return "Invalid System ID";

    case ESME_RCANCELFAIL:
        return "Cancel SM Failed";

    case ESME_RREPLACEFAIL:
        return "Replace SM Failed";

    case ESME_RMSGQFUL:
        return "Message Queue Full";

    case ESME_RINVSERTYP:
        return "Invalid Service Type";

    case ESME_RINVNUMDESTS:
        return "Invalid number of destinations";

    case ESME_RINVDLNAME:
        return "Invalid Distribution List name";

    case ESME_RINVDESTFLAG:
        return "Destination flag (submit_multi)";

    case ESME_RINVSUBREP:
        return "Invalid ‘submit with replace’ request (i.e. submit_sm with replace_if_present_flag set)";

    case ESME_RINVESMSUBMIT:
        return "Invalid esm_SUBMIT field data";

    case ESME_RCNTSUBDL:
        return "Cannot Submit to Distribution List";

    case ESME_RSUBMITFAIL:
        return "submit_sm or submit_multi failed";

    case ESME_RINVSRCTON:
        return "Invalid Source address TON";

    case ESME_RINVSRCNPI:
        return "Invalid Source address NPI";

    case ESME_RINVDSTTON:
        return "Invalid Destination address TON";

    case ESME_RINVDSTNPI:
        return "Invalid Destination address NPI";

    case ESME_RINVSYSTYP:
        return "Invalid system_type field";

    case ESME_RINVREPFLAG:
        return "Invalid replace_if_present flag";

    case ESME_RINVNUMMSGS:
        return "Invalid number of messages";

    case ESME_RTHROTTLED:
        return "Throttling error (ESME has exceeded allowed message limits)";

    case ESME_RINVSCHED:
        return "Invalid Scheduled Delivery Time";

    case ESME_RINVEXPIRY:
        return "Invalid message (Expiry time)";

    case ESME_RINVDFTMSGID:
        return "Predefined Message Invalid or Not Found";

    case ESME_RX_T_APPN:
        return "ESME Receiver Temporary App Error Code";

    case ESME_RX_P_APPN:
        return "ESME Receiver Permanent App Error Code";

    case ESME_RX_R_APPN:
        return "ESME Receiver Reject Message Error Code";

    case ESME_RQUERYFAIL:
        return "query_sm request failed";

    case ESME_RINVOPTPARSTREAM:
        return "Error in the optional part of the PDU Body.";

    case ESME_ROPTPARNOTALLWD:
        return "Optional Parameter not allowed";

    case ESME_RINVPARLEN:
        return "Invalid Parameter Length.";

    case ESME_RMISSINGOPTPARAM:
        return "Expected Optional Parameter missing";

    case ESME_RINVOPTPARAMVAL:
        return "Invalid Optional Parameter Value";

    case ESME_RDELIVERYFAILURE:
        return "Delivery Failure (data_sm_resp)";

    case ESME_RUNKNOWNERR:
        return "Unknown Error";

    default:
        return "Unknown";
    }
}
}  // namespace smpp
