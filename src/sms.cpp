#include "sms.h"
#include <iostream>

using namespace std;

std::ostream& smpp::operator<<(std::ostream& out, smpp::SMS& sms)
{
	if (sms.null) {
		out << "sms values: NULL" << endl;
		return out;
	}

	out << "sms values:" << endl;
	out << "service_type: " << sms.service_type << endl;
	out << "source_addr_ton:" << sms.source_addr_ton << endl;
	out << "source_addr_npi:" << sms.source_addr_npi << endl;
	out << "source_addr: " << sms.source_addr << endl;
	out << "dest_addr_ton:" << sms.dest_addr_ton << endl;
	out << "dest_addr_npi:" << sms.dest_addr_npi << endl;
	out << "dest_addr:" << sms.dest_addr << endl;

	out << "esm_class;" << sms.esm_class << endl;
	out << "protocol_id:" << sms.protocol_id << endl;
	out << "priority_flag:" << sms.priority_flag << endl;
	out << "schedule_delivery_time:" << sms.schedule_delivery_time << endl;
	out << "validity_period:" << sms.validity_period << endl;
	out << "registered_delivery    :" << sms.registered_delivery << endl;
	out << "replace_if_present_flag:" << sms.replace_if_present_flag << endl;

	out << "data_coding:" << sms.data_coding << endl;
	out << "sm_default_msg_id:" << sms.sm_default_msg_id << endl;
	out << "sm_length:" << sms.sm_length << endl;
	out << "short_message:" << sms.short_message << std::endl;
	return out;
}


