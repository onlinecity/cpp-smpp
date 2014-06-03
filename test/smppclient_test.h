//
// Copyright (C) 2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
//

#pragma once

#include <asio.hpp>
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "smpp/smppclient.h"
#include "test_flags.h"

class SmppClientTest: public testing::Test {
 public:
  asio::ip::tcp::endpoint endpoint_;
  asio::io_service ios_;
  std::shared_ptr<asio::ip::tcp::socket> socket_;
  std::shared_ptr<smpp::SmppClient> client_;

  SmppClientTest() :
    endpoint_(asio::ip::address_v4::from_string(FLAGS_host), FLAGS_port),
    ios_(),
    socket_(std::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(ios_))),
    client_(std::shared_ptr<smpp::SmppClient>(new smpp::SmppClient(socket_))) {
  }

  virtual void SetUp() {
    FLAGS_v = 1;
  }

  virtual void TearDown() {
    if (client_->IsBound()) {
      client_->Unbind();
    }

    socket_->close();
  }
};

class SmppClientCsmsTest : public testing::Test {
 public:
  asio::ip::tcp::endpoint endpoint_;
  asio::io_service ios_;
  std::shared_ptr<asio::ip::tcp::socket> socket_;
  smpp::SmppClient client_;
  smpp::SmppAddress sender_;
  smpp::SmppAddress receiver_;
  std::string message_170_;
  std::string message_256_;
  std::string message_300_;
  std::string message_400_;
  std::string message_500_;

  SmppClientCsmsTest() :
    endpoint_(asio::ip::address_v4::from_string(FLAGS_host), FLAGS_port),
    ios_(),
    socket_(std::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(ios_))),
    client_(socket_),
    sender_(FLAGS_sender, smpp::TON::ALPHANUMERIC, smpp::NPI::UNKNOWN),
    receiver_(FLAGS_receiver, smpp::TON::INTERNATIONAL, smpp::NPI::E164),
    message_170_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Ut blandit sagittis erat, id tempus enim dapibus vitae. Duis "
        "posuere nunc a augue tincidunt laoreet. Ut volutpat."),
    message_256_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Curabitur ultricies, tellus vel semper placerat, augue est faucibus "
        "justo, eget porta purus ligula sit amet diam. Maecenas pretium dictum "
        "ante, eu volutpat nunc imperdiet sed. Fusce sodales volutpat."),
    message_300_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Morbi ullamcorper turpis vitae justo pharetra, et semper nisi feugiat. "
        "Aliquam suscipit dui eu odio gravida sagittis. Duis ut erat quis odio "
        "tristique sagittis. Nam orci dolor, adipiscing sed tincidunt et, "
        "vulputate eu mi. In sodales volutpat."),
    message_400_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Fusce ultricies nec purus nec faucibus. Sed dictum ante lobortis "
        "sapien mattis, sed scelerisque quam auctor. Maecenas eget imperdiet "
        "turpis. Morbi laoreet sed urna vitae porta. Mauris in metus vestibulum, "
        "pretium metus tincidunt, lobortis elit. Aenean mollis vel urna a pharetra. "
        "Duis et venenatis velit. Donec a dignissim massa. Duis posuere."),
    message_500_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Maecenas consequat massa eget condimentum auctor. Integer vel eros quis "
        "purus malesuada dapibus eu at mauris. Ut venenatis lorem sed orci mattis, "
        "eu sodales ipsum venenatis. Nulla facilisi. Aenean convallis ut sapien in "
        "euismod. Nam condimentum turpis a nisi iaculis hendrerit. Aenean magna urna, "
        "vestibulum id molestie vitae, dignissim eu odio. Pellentesque in est in felis"
        " faucibus faucibus sed non libero. Cras ultricies faucibus velit sed.") {
  }

  virtual void SetUp() {
    socket_->connect(endpoint_);
    client_.BindTransmitter(FLAGS_username, FLAGS_password);
  }

  virtual void TearDown() {
    if (client_.IsBound()) {
      client_.Unbind();
    }

    socket_->close();
  }
};
