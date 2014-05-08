/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 */
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <list>
#include <string>
#include <tuple>

#include "./asio.hpp"
#include "./smppclient_test.h"
#include "gtest/gtest.h"
#include "smpp/gsmencoding.h"
#include "smpp/smpp_params.h"
#include "smpp/timeformat.h"

using smpp::SmppAddress;
using smpp::SmppClient;
using smpp::TLV;
using smpp::encoding::GsmEncoder;
using std::list;
using std::string;

//  Test login of either transmitter or receiver
TEST_F(SmppClientTest, BindReceiver) {
  socket->connect(endpoint);
  ASSERT_TRUE(socket->is_open());
  client->BindReceiver(SMPP_USERNAME, SMPP_PASSWORD);
  ASSERT_TRUE(client->IsBound());
  client->Unbind();
  socket->close();
}

TEST_F(SmppClientTest, ReceiveSms) {
  socket->connect(endpoint);
  client->BindReceiver(SMPP_USERNAME, SMPP_PASSWORD);
  LOG(INFO) << "Waiting for smpp connection to send message";
  smpp::SMS sms = client->ReadSms();
}
