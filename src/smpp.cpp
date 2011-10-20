#include "smpp.h"

using namespace std;
namespace smapp {

string getEsmeStatus(uint32_t i)
{
	switch (i) {
		case smpp::ESME_ROK:
			return "No Error";
		case smpp::ESME_RINVMSGLEN:
			return "Message Length is invalid";
		case smpp::ESME_RINVCMDLEN:
			return "Command Length is invalid";
		case smpp::ESME_RINVCMDID:
			return "Invalid Command ID";
		case smpp::ESME_RINVBNDSTS:
			return "Incorrect BIND Status for given command";
		case smpp::ESME_RALYBND:
			return "ESME Already in Bound State";
		case smpp::ESME_RINVPRTFLG:
			return "Invalid Priority Flag";
		case smpp::ESME_RINVREGDLVFLG:
			return "Invalid Registered Delivery Flag";
		case smpp::ESME_RSYSERR:
			return "System Error";
		case smpp::ESME_RINVSRCADR:
			return "Invalid Source Address";
		case smpp::ESME_RINVDSTADR:
			return "Invalid Dest Addr";
		case smpp::ESME_RINVMSGID:
			return "Message ID is invalid";
		case smpp::ESME_RBINDFAIL:
			return "Bind Failed";
		case smpp::ESME_RINVPASWD:
			return "Invalid Password";
		case smpp::ESME_RINVSYSID:
			return "Invalid System ID";
		case smpp::ESME_RCANCELFAIL:
			return "Cancel SM Failed";
		case smpp::ESME_RREPLACEFAIL:
			return "Replace SM Failed";
		case smpp::ESME_RMSGQFUL:
			return "Message Queue Full";
		case smpp::ESME_RINVSERTYP:
			return "Invalid Service Type";
		case smpp::ESME_RINVNUMDESTS:
			return "Invalid number of destinations";
		case smpp::ESME_RINVDLNAME:
			return "Invalid Distribution List name";
		case smpp::ESME_RINVDESTFLAG:
			return "Destination flag (submit_multi)";
		case smpp::ESME_RINVSUBREP:
			return "Invalid ‘submit with replace’ request (i.e. submit_sm with replace_if_present_flag set)";
		case smpp::ESME_RINVESMSUBMIT:
			return "Invalid esm_SUBMIT field data";
		case smpp::ESME_RCNTSUBDL:
			return "Cannot Submit to Distribution List";
		case smpp::ESME_RSUBMITFAIL:
			return "submit_sm or submit_multi failed";
		case smpp::ESME_RINVSRCTON:
			return "Invalid Source address TON";
		case smpp::ESME_RINVSRCNPI:
			return "Invalid Source address NPI";
		case smpp::ESME_RINVDSTTON:
			return "Invalid Destination address TON";
		case smpp::ESME_RINVDSTNPI:
			return "Invalid Destination address NPI";
		case smpp::ESME_RINVSYSTYP:
			return "Invalid system_type field";
		case smpp::ESME_RINVREPFLAG:
			return "Invalid replace_if_present flag";
		case smpp::ESME_RINVNUMMSGS:
			return "Invalid number of messages";
		case smpp::ESME_RTHROTTLED:
			return "Throttling error (ESME has exceeded allowed message limits)";
		case smpp::ESME_RINVSCHED:
			return "Invalid Scheduled Delivery Time";
		case smpp::ESME_RINVEXPIRY:
			return "Invalid message (Expiry time)";
		case smpp::ESME_RINVDFTMSGID:
			return "Predefined Message Invalid or Not Found";
		case smpp::ESME_RX_T_APPN:
			return "ESME Receiver Temporary App Error Code";
		case smpp::ESME_RX_P_APPN:
			return "ESME Receiver Permanent App Error Code";
		case smpp::ESME_RX_R_APPN:
			return "ESME Receiver Reject Message Error Code";
		case smpp::ESME_RQUERYFAIL:
			return "query_sm request failed";
		case smpp::ESME_RINVOPTPARSTREAM:
			return "Error in the optional part of the PDU Body.";
		case smpp::ESME_ROPTPARNOTALLWD:
			return "Optional Parameter not allowed";
		case smpp::ESME_RINVPARLEN:
			return "Invalid Parameter Length.";
		case smpp::ESME_RMISSINGOPTPARAM:
			return "Expected Optional Parameter missing";
		case smpp::ESME_RINVOPTPARAMVAL:
			return "Invalid Optional Parameter Value";
		case smpp::ESME_RDELIVERYFAILURE:
			return "Delivery Failure (data_sm_resp)";
		case smpp::ESME_RUNKNOWNERR:
			return "Unknown Error";

		default:
			return "Unknown";
	}
}
}
