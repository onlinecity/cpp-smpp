/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 */
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <chrono>
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
  socket_->connect(endpoint_);
  ASSERT_TRUE(socket_->is_open());
  client_->BindReceiver(FLAGS_username, FLAGS_password);
  ASSERT_TRUE(client_->IsBound());
  client_->Unbind();
  socket_->close();
}

TEST_F(SmppClientTest, ReceiveSmsTimeout) {
  int socket_read_timeout = FLAGS_socket_read_timeout;  // Save defaults
  int timeout = 500;  // timeout 500 ms
  FLAGS_socket_read_timeout = timeout;  // Set timeout
  socket_->connect(endpoint_);
  client_->BindReceiver(FLAGS_username, FLAGS_password);
  // Retrieve all messages and wait for timeout
  smpp::SMS sms;

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  do {
    start = std::chrono::high_resolution_clock::now();
    sms = client_->ReadSms();
    end = std::chrono::high_resolution_clock::now();
  } while (!sms.is_null);

  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  EXPECT_GE(ms.count(), timeout);
  EXPECT_LT(ms.count(), timeout + 100);  // Timeout + 100 ms
  FLAGS_socket_read_timeout = socket_read_timeout;  // Restore defaults
}

TEST_F(SmppClientTest, ReceiveSms) {
  socket_->connect(endpoint_);
  client_->BindReceiver(FLAGS_username, FLAGS_password);
  LOG(INFO) << "Waiting for smpp connection to send message";
  smpp::SMS sms = client_->ReadSms();
}

