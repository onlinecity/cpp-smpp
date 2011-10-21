#ifndef SMS_H_
#define SMS_H_

#include <list>
#include <iostream>
#include <sstream>
#include <locale>
#include <boost/regex.hpp>
#include <boost/shared_array.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include "smpp.h"
#include "pdu.h"
#include "tlv.h"
#include "timeformat.h"

namespace smpp {
/**
 * Class representing an SMS.
 */
class SMS
{
public:
	std::string service_type;
	int source_addr_ton;
	int source_addr_npi;
	std::string source_addr;

	int dest_addr_ton;
	int dest_addr_npi;
	std::string dest_addr;

	int esm_class;
	int protocol_id;
	int priority_flag;

	std::string schedule_delivery_time;
	std::string validity_period;

	int registered_delivery;
	int replace_if_present_flag;

	int data_coding;
	int sm_default_msg_id;
	int sm_length;

	std::string short_message;

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
			pdu >> len;

			if (tag == 0) {
				break;
			}

			if (len == 0) {
				tlvs.push_back(TLV(tag));
				continue;
			}

			boost::shared_array<uint8_t> octets(new uint8_t[len]);
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

	friend std::ostream &operator <<(std::ostream &, const SMS &);

};
// SMS class

/**
 * Class representing a Delivery Report.
 */
class DeliveryReport: public SMS
{

public:
	std::string id;
	uint32_t sub;
	uint32_t dlvrd;
	boost::posix_time::ptime submitDate;
	boost::posix_time::ptime doneDate;
	std::string stat;
	std::string err;
	std::string text;

	DeliveryReport() :
			SMS(), id(""), sub(0), dlvrd(0), submitDate(), doneDate(), stat(""), err(""), text("")
	{
	}

	/**
	 * Constructs a delivery report from an SMS.
	 * @param sms SMS to construct delivery report from.
	 */
	DeliveryReport(const smpp::SMS &sms) :
			smpp::SMS(sms), id(""), sub(0), dlvrd(0), submitDate(), doneDate(), stat(""), err(""), text("")
	{
		using namespace boost;
		regex expression(
				"^id:([^ ]+) sub:(\\d{1,3}) dlvrd:(\\d{3}) submit date:(\\d{10}) done date:(\\d{10}) stat:([A-Z]{7}) err:(\\d{3}) text:(.*)$",
				regex_constants::perl);
		smatch what;

		if (regex_match(short_message, what, expression)) {
			id = what[1];
			sub = boost::lexical_cast<uint32_t>(what[2]);
			dlvrd = boost::lexical_cast<uint32_t>(what[3]);

			submitDate = smpp::timeformat::parseDlrTimestamp(what[4]);
			doneDate = smpp::timeformat::parseDlrTimestamp(what[5]);

			stat = what[6];
			err = what[7];
			text = what[8];
		}
	}

	DeliveryReport(const DeliveryReport &rhs) :
					smpp::SMS(rhs),
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
};

} // namespace smpp

#endif /* SMS_H_ */
