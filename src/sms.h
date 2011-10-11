#ifndef SMS_H_
#define SMS_H_

#include <list>
#include <iostream>
#include <sstream>
#include <locale>
#include <boost/regex.hpp>
#include <boost/shared_array.hpp>

#include "smpp.h"
#include "pdu.h"
#include "tlv.h"

using namespace std;
using namespace boost;

namespace smpp {
/**
 * Class representing an SMS.
 */
class SMS
{
public:
	string service_type;
	int source_addr_ton;
	int source_addr_npi;
	string source_addr;

	int dest_addr_ton;
	int dest_addr_npi;
	string dest_addr;

	int esm_class;
	int protocol_id;
	int priority_flag;
	string schedule_delivery_time;
	string validity_period;
	int registered_delivery;
	int replace_if_present_flag;

	int data_coding;
	int sm_default_msg_id;
	int sm_length;

	string short_message;

	std::list<TLV> tlvs;

	bool null;

	SMS() :
			service_type(""),
			source_addr_ton(0),
			source_addr_npi(0),
			source_addr(""),
			dest_addr_ton(0),
			dest_addr_npi(0),
			dest_addr(""),
			esm_class(0),
			protocol_id(0),
			priority_flag(0),
			schedule_delivery_time(""),
			validity_period(""),
			registered_delivery(0),
			replace_if_present_flag(0),
			data_coding(0),
			sm_default_msg_id(0),
			sm_length(0),
			short_message(""),
			tlvs(),
			null(true)
	{

	}

	virtual ~SMS()
	{

	}

	SMS(PDU &pdu) :
			service_type(""),
			source_addr_ton(0),
			source_addr_npi(0),
			source_addr(""),
			dest_addr_ton(0),
			dest_addr_npi(0),
			dest_addr(""),
			esm_class(0),
			protocol_id(0),
			priority_flag(0),
			schedule_delivery_time(""),
			validity_period(""),
			registered_delivery(0),
			replace_if_present_flag(0),
			data_coding(0),
			sm_default_msg_id(0),
			sm_length(0),
			short_message(""),
			tlvs(),
			null(false)

	{
		pdu >> service_type;

		pdu >> source_addr_ton;
		pdu >> source_addr_npi;
		pdu >> source_addr;

		pdu >> dest_addr_ton;
		pdu >> dest_addr_npi;
		pdu >> dest_addr;

		pdu >> esm_class;
		pdu >> protocol_id;
		pdu >> priority_flag;
		pdu >> schedule_delivery_time;
		pdu >> validity_period;
		pdu >> registered_delivery;
		pdu >> replace_if_present_flag;
		pdu >> data_coding;
		pdu >> sm_default_msg_id;
		pdu >> sm_length;

		pdu >> short_message;

		// fetch any optional tags
		uint16_t len = 0;
		uint16_t tag = 0;

		while (pdu.hasMoreData()) {
			pdu >> tag;
			if (len == 0) {
				tlvs.push_back(TLV(tag));
				continue;
			}

			pdu >> len;
			shared_array<uint8_t> octets(new uint8_t[len]);
			pdu.readOctets(octets, len);
			tlvs.push_back(TLV(tag, len, octets));
		}
	}

	SMS(const SMS &sms) :
			service_type(sms.service_type),
			source_addr_ton(sms.source_addr_ton),
			source_addr_npi(sms.source_addr_npi),
			source_addr(sms.source_addr),
			dest_addr_ton(sms.dest_addr_ton),
			dest_addr_npi(sms.dest_addr_npi),
			dest_addr(sms.dest_addr),
			esm_class(sms.esm_class),
			protocol_id(sms.protocol_id),
			priority_flag(sms.priority_flag),
			schedule_delivery_time(sms.schedule_delivery_time),
			validity_period(sms.validity_period),
			registered_delivery(sms.registered_delivery),
			replace_if_present_flag(sms.replace_if_present_flag),
			data_coding(sms.data_coding),
			sm_default_msg_id(sms.sm_default_msg_id),
			sm_length(sms.sm_length),
			short_message(sms.short_message),
			tlvs(),
			null(sms.null)
	{
		if (!null) {
			std::copy(sms.tlvs.begin(), sms.tlvs.end(), tlvs.begin());
		}
	}

	const SMS & operator=(const smpp::SMS &sms)
	{
		if (this != &sms) {
			service_type = sms.service_type;
			source_addr_ton = sms.source_addr_ton;
			source_addr_npi = sms.source_addr_npi;
			source_addr = sms.source_addr;
			dest_addr_ton = sms.dest_addr_ton;

			dest_addr_npi = sms.dest_addr_npi;
			dest_addr = sms.dest_addr;
			esm_class = sms.esm_class;
			protocol_id = sms.protocol_id;
			priority_flag = sms.priority_flag;

			schedule_delivery_time = sms.schedule_delivery_time;
			validity_period = sms.validity_period;
			registered_delivery = sms.registered_delivery;

			replace_if_present_flag = sms.replace_if_present_flag;
			data_coding = sms.data_coding;
			sm_default_msg_id = sms.sm_default_msg_id;
			sm_length = sms.sm_length;

			null = sms.null;

			if (!null) {
//				short_message = new uint8_t[sm_length + 1];std::copy(sms.short_message, sms.short_message + sm_length + 1,short_message);
				std::copy(sms.tlvs.begin(), sms.tlvs.end(), tlvs.begin());
			}
		}

		return *this;
	}

	friend std::ostream &operator << (std::ostream &, const SMS &);

};
// SMS class

/**
 * Class representing a Delivery Report.
 */
class DeliveryReport: public SMS
{

public:
	string id;
	uint8_t sub;
	uint8_t dlvrd;
	string submitDate;
	string doneDate;
	string stat;
	string err;
	string text;

	DeliveryReport() :
			SMS(), id(""), sub(0), dlvrd(0), submitDate(""), doneDate(""), stat(""), err(""), text("")
	{
	}

	/**
	 * Constructs a delivery report from an SMS.
	 * @param sms SMS to construct delivery report from.
	 */
	DeliveryReport(const smpp::SMS &sms) :
			smpp::SMS(sms), id(""), sub(0), dlvrd(0), submitDate(""), doneDate(""), stat(""), err(""), text("")
	{
		using namespace boost;
		regex expression(
				"^id:([^ ]+) sub:(\\d{1,3}) dlvrd:(\\d{3}) submit date:(\\d{10}) done date:(\\d{10}) stat:([A-Z]{7}) err:(\\d{3}) text:(.*)$",
				regex_constants::perl);
		cmatch what;

		if (regex_match(short_message.c_str(), what, expression)) {
			id = what[1];
			sub = atoi(((string) what[2]).c_str());
			dlvrd = atoi(((string) what[3]).c_str());
			submitDate = what[4];
			doneDate = what[5];

			stat = what[6];
			err = what[7];
			text = what[8];
		}
	}

	DeliveryReport(const DeliveryReport &rhs) :
			id(rhs.id),
			sub(rhs.sub),
			dlvrd(rhs.dlvrd),
			submitDate(rhs.submitDate),
			doneDate(rhs.doneDate),
			stat(rhs.stat),
			err(rhs.err),
			text(rhs.text)
	{
	}

	const DeliveryReport & operator=(const DeliveryReport &rhs)
	{
		if (this != &rhs) {
			id = rhs.id;
			sub = rhs.sub;
			dlvrd = rhs.dlvrd;
			submitDate = rhs.submitDate;
			doneDate = rhs.doneDate;
			stat = rhs.stat;
			err = rhs.err;
			text = rhs.text;
		}
		return *this;
	}
};

} // namespace smpp

#endif /* SMS_H_ */
