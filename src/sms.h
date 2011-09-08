#ifndef SMS_H_
#define SMS_H_

#include <list>
#include <iostream>
#include <sstream>
#include <locale>
#include <boost/regex.hpp>

#include "smpp.h"
#include "pdu.h"
#include "tlv.h"

using namespace std;

namespace smpp {
/**
 * Class representing an SMS.
 */
class SMS {
public:
	uint8_t *service_type;
	int source_addr_ton;
	int source_addr_npi;
	uint8_t *source_addr;

	int dest_addr_ton;
	int dest_addr_npi;
	uint8_t *dest_addr;

	int esm_class;
	int protocol_id;
	int priority_flag;
	uint8_t *schedule_delivery_time;
	uint8_t *validity_period;
	int registered_delivery;
	int replace_if_present_flag;

	int data_coding;
	int sm_default_msg_id;
	int sm_length;

	uint8_t *short_message;

	std::list<TLV> tlvs;

	bool null;

	SMS() :
			null(true) {

	}

	SMS(PDU &pdu) :
			null(false)

	{
		service_type = pdu.readStr();

		source_addr_ton = pdu.readInt();
		source_addr_npi = pdu.readInt();
		source_addr = pdu.readStr();

		dest_addr_ton = pdu.readInt();
		dest_addr_npi = pdu.readInt();
		dest_addr = pdu.readStr();

		esm_class = pdu.readInt();
		protocol_id = pdu.readInt();
		priority_flag = pdu.readInt();
		schedule_delivery_time = pdu.readStr();
		validity_period = pdu.readStr();
		registered_delivery = pdu.readInt();
		replace_if_present_flag = pdu.readInt();
		data_coding = pdu.readInt();
		sm_default_msg_id = pdu.readInt();
		sm_length = pdu.readInt();

		short_message = new uint8_t[sm_length + 1];

pdu		.readStr(short_message, sm_length);

		// fetch any optional tags
		uint16_t len = 0;
		uint16_t tag = 0;
		uint8_t* octets;

		while (pdu.hasMoreData()) {
			tag = pdu.read2Int();
			if (len == 0) {
				tlvs.push_back(TLV(tag));
				continue;
			}

			len = pdu.read2Int();
			octets = new uint8_t[len];
			pdu.readOctets(octets, len);
			tlvs.push_back(TLV(tag, len, octets));
		}
	}

	SMS(const smpp::SMS &sms) :
			service_type(sms.service_type), source_addr_ton(sms.source_addr_ton), source_addr_npi(
					sms.source_addr_npi), source_addr(sms.source_addr), dest_addr_ton(
					sms.dest_addr_ton), dest_addr_npi(sms.dest_addr_npi), dest_addr(
					sms.dest_addr), esm_class(sms.esm_class), protocol_id(
					sms.protocol_id), priority_flag(sms.priority_flag), schedule_delivery_time(
					sms.schedule_delivery_time), validity_period(sms.validity_period), registered_delivery(
					sms.registered_delivery), replace_if_present_flag(
					sms.replace_if_present_flag), data_coding(sms.data_coding), sm_default_msg_id(
					sms.sm_default_msg_id), sm_length(sms.sm_length), null(sms.null) {
		if (!null) {
			short_message = new uint8_t[sm_length + 1];std::copy(sms.short_message, sms.short_message + sm_length + 1, short_message);
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
					short_message = new uint8_t[sm_length + 1];
					std::copy(sms.short_message, sms.short_message + sm_length + 1, short_message);
					std::copy(sms.tlvs.begin(), sms.tlvs.end(), tlvs.begin());
				}
			}

			return *this;
		}

		~SMS()
		{
			if (!null) delete[] short_message;

		}

		friend std::ostream& smpp::operator<<(std::ostream&, smpp::SMS&);

	}; // SMS class

	/**
	 * Class representing a Delivery Report.
	 */
class DeliveryReport: public SMS {

public:
	string id;
	uint8_t sub;
	uint8_t dlvrd;
	string submitDate;
	string doneDate;
	string stat;
	string err;
	string text;

	/**
	 * Constructs a delivery report from an SMS.
	 * @param sms SMS to construct delivery report from.
	 */
	DeliveryReport(const smpp::SMS &sms) :
			SMS(sms)

	{
		using namespace boost;
//		using namespace boost::posix_time;

		char* msg = reinterpret_cast<char*>(short_message);
		regex expression(
				"^id:([^ ]+) sub:(\\d{1,3}) dlvrd:(\\d{3}) submit date:(\\d{10}) done date:(\\d{10}) stat:([A-Z]{7}) err:(\\d{3}) text:(.*)$",
				regex_constants::perl);
		cmatch what;

		if (regex_match(msg, what, expression)) {
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
};

} // namespace smpp

#endif /* SMS_H_ */
