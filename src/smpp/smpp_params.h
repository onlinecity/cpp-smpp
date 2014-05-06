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
    esm_class(smpp::ESM::SUBMIT_MODE_SMSC_DEFAULT),
    protocol_id(0),
    priority_flag(0),
    schedule_delivery_time(""),
    validity_period(""),
    registered_delivery(0),
    replace_if_present_flag(0),
    data_coding(smpp::DATA_CODING_DEFAULT),
    sm_default_msg_id(0)
    {
  }

  // ESME transmitter options
  std::string service_type;
  smpp::ESM esm_class;
  uint8_t protocol_id;
  uint8_t priority_flag;
  std::string schedule_delivery_time;
  std::string validity_period;
  uint8_t registered_delivery;
  uint8_t replace_if_present_flag;
  uint8_t data_coding;
  uint8_t sm_default_msg_id;

};
}  // namespace smpp
