/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "smpp/sms.h"
#include <regex>
#include <string>

using std::endl;
using std::streamsize;
using std::stoi;
using std::string;

namespace smpp {
SMS::SMS() :
    service_type(""), /**/
    source_addr_ton(0), /**/
    source_addr_npi(0), /**/
    source_addr(""), /**/
    dest_addr_ton(0), /**/
    dest_addr_npi(0), /**/
    dest_addr(""), /**/
    esm_class(0), /**/
    protocol_id(0), /**/
    priority_flag(0), /**/
    schedule_delivery_time(""), /**/
    validity_period(""), /**/
    registered_delivery(0), /**/
    replace_if_present_flag(0), /**/
    data_coding(0), /**/
    sm_default_msg_id(0), /**/
    sm_length(0), /**/
    short_message(""), /**/
    tlvs(), /**/
    is_null(true) {
}

SMS::SMS(PDU &pdu) :
    service_type(""), /**/
    source_addr_ton(0), /**/
    source_addr_npi(0), /**/
    source_addr(""), /**/
    dest_addr_ton(0), /**/
    dest_addr_npi(0), /**/
    dest_addr(""), /**/
    esm_class(0), /**/
    protocol_id(0), /**/
    priority_flag(0), /**/
    schedule_delivery_time(""), /**/
    validity_period(""), /**/
    registered_delivery(0), /**/
    replace_if_present_flag(0), /**/
    data_coding(0), /**/
    sm_default_msg_id(0), /**/
    sm_length(0), /**/
    short_message(""), /**/
    tlvs(), /**/
    is_null(false) {
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
    // read short_message with readOctets to ensure we get all chars including null bytes
    boost::shared_array<uint8_t> msg(new uint8_t[sm_length]);
    pdu.readOctets(msg, boost::numeric_cast<streamsize>(sm_length));
    short_message = string(reinterpret_cast<char*>(msg.get()), boost::numeric_cast<size_t>(sm_length));
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

SMS::SMS(const SMS &rhs) :
    service_type(rhs.service_type), /**/
    source_addr_ton(rhs.source_addr_ton), /**/
    source_addr_npi(rhs.source_addr_npi), /**/
    source_addr(rhs.source_addr), /**/
    dest_addr_ton(rhs.dest_addr_ton), /**/
    dest_addr_npi(rhs.dest_addr_npi), /**/
    dest_addr(rhs.dest_addr), /**/
    esm_class(rhs.esm_class), /**/
    protocol_id(rhs.protocol_id), /**/
    priority_flag(rhs.priority_flag), /**/
    schedule_delivery_time(rhs.schedule_delivery_time), /**/
    validity_period(rhs.validity_period), /**/
    registered_delivery(rhs.registered_delivery), /**/
    replace_if_present_flag(rhs.replace_if_present_flag), /**/
    data_coding(rhs.data_coding), /**/
    sm_default_msg_id(rhs.sm_default_msg_id), /**/
    sm_length(rhs.sm_length), /**/
    short_message(rhs.short_message), /**/
    tlvs(), /**/
    is_null(rhs.is_null) {
    if (!is_null) {
        tlvs.assign(rhs.tlvs.begin(), rhs.tlvs.end());
    }
}

DeliveryReport::DeliveryReport() :
    SMS(),
    id(""),
    sub(0),
    dlvrd(0),
    submitDate(),
    doneDate(),
    stat(""),
    err(""),
    text("") {
}

DeliveryReport::DeliveryReport(const SMS &sms) :
    SMS(sms), /**/
    id(""), /**/
    sub(0), /**/
    dlvrd(0), /**/
    submitDate(), /**/
    doneDate(), /**/
    stat(""), /**/
    err(""), /**/
    text("") {
    std::regex expression(
        "^id:([^ ]+)\\s+sub:(\\d{1,3})\\s+dlvrd:(\\d{1,3})\\s+submit\\s+date:(\\d{1,10})\\s+done\\s+date:(\\d{1,10})\\s+stat:([A-Z]{7})\\s+err:(\\d{1,3})\\s+text:(.*)$");
    std::smatch what;

    if (std::regex_match(short_message, what, expression)) {
        id = what[1];
        sub = stoi(what[2]);
        dlvrd = stoi(what[3]);
        submitDate = smpp::timeformat::parseDlrTimestamp(what[4]);
        doneDate = smpp::timeformat::parseDlrTimestamp(what[5]);
        stat = what[6];
        err = what[7];
        text = what[8];
    }
}

DeliveryReport::DeliveryReport(const DeliveryReport &rhs) :
    smpp::SMS(rhs), /**/
    id(rhs.id), /**/
    sub(rhs.sub), /**/
    dlvrd(rhs.dlvrd), /**/
    submitDate(rhs.submitDate), /**/
    doneDate(rhs.doneDate), /**/
    stat(rhs.stat), /**/
    err(rhs.err), /**/
    text(rhs.text) {
}
}  // namespace smpp

std::ostream &smpp::operator<<(std::ostream &out, smpp::SMS &sms) {
    if (sms.is_null) {
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

