//
// Copyright (C) 2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
//

#pragma once

#include <asio.hpp>
#include <memory>

#include "./connectionsetting.h"
#include "gtest/gtest.h"
#include "smpp/smppclient.h"

class SmppClientTest: public testing::Test {
 public:
  asio::ip::tcp::endpoint endpoint;
  asio::io_service ios;
  std::shared_ptr<asio::ip::tcp::socket> socket;
  std::shared_ptr<smpp::SmppClient> client;

  SmppClientTest() :
    endpoint(asio::ip::address_v4::from_string(SMPP_HOST), SMPP_PORT),
    ios(),
    socket(std::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(ios))),
    client(std::shared_ptr<smpp::SmppClient>(new smpp::SmppClient(socket))) {
  }

  virtual void SetUp() {
    FLAGS_v = 1;
  }

  virtual void TearDown() {
    if (client->IsBound()) {
      client->Unbind();
    }

    socket->close();
  }
};
