//
// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk
//

#pragma once

#include <cstdint>
#include <string>
#include "smpp/smpp.h"

namespace smpp {
struct SmppParams {
  SmppParams() :
    service_type(""),
    esm_class(ESM::SUBMIT_MODE_SMSC_DEFAULT),
    protocol_id(0),
    priority_flag(Priority::GSM_NON_PRIORITY),
    schedule_delivery_time(""),
    validity_period(""),
    registered_delivery(RegisteredDelivery::DELIVERY_NO),
    replace_if_present_flag(ReplaceIfPresent::REPLACE_NO),
    data_coding(DataCoding::DEFAULT),
    sm_default_msg_id(0) {
  }

  // ESME transmitter options
  std::string service_type;
  smpp::ESM esm_class;
  uint8_t protocol_id;
  smpp::Priority priority_flag;
  std::string schedule_delivery_time;
  std::string validity_period;
  smpp::RegisteredDelivery registered_delivery;
  smpp::ReplaceIfPresent replace_if_present_flag;
  smpp::DataCoding data_coding;
  uint8_t sm_default_msg_id;
};
}  // namespace smpp
