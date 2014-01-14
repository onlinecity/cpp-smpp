/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_SMS_H_
#define SMPP_SMS_H_

#include <boost/shared_array.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <list>
#include <string>

#include "smpp/smpp.h"
#include "smpp/pdu.h"
#include "smpp/tlv.h"
#include "smpp/timeformat.h"

namespace smpp {
/**
 * Class representing an SMS.
 */
class SMS {
  public:
    std::string service_type;
    int source_addr_ton;
    int source_addr_npi;
    std::string source_addr;

    int dest_addr_ton;
    int dest_addr_npi;
    std::string dest_addr;

    uint8_t esm_class;
    int protocol_id;
    int priority_flag;

    std::string schedule_delivery_time;
    std::string validity_period;

    int registered_delivery;
    int replace_if_present_flag;

    uint8_t data_coding;
    int sm_default_msg_id;
    int sm_length;

    std::string short_message;
    std::list<TLV> tlvs;

    bool is_null;

    SMS();
    virtual ~SMS() {
    }
    explicit SMS(PDU &pdu);
    SMS(const SMS &sms);
};
std::ostream &operator<<(std::ostream &, smpp::SMS &);
// SMS class

/**
 * Class representing a Delivery Report.
 */
class DeliveryReport: public SMS {
  public:
    std::string id;
    uint32_t sub;
    uint32_t dlvrd;
    boost::posix_time::ptime submitDate;
    boost::posix_time::ptime doneDate;
    std::string stat;
    std::string err;
    std::string text;

    DeliveryReport();

    /**
     * Constructs a delivery report from an SMS.
     * @param sms SMS to construct delivery report from.
     */
    explicit DeliveryReport(const smpp::SMS &sms);

    DeliveryReport(const DeliveryReport &rhs);
};
}  // namespace smpp
#endif  // SMPP_SMS_H_
