//
// Copyright (C) 2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
//

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <ctime>
#include <list>
#include <string>
#include <tuple>

#include "./smppclient_test.h"
#include "asio.hpp"
#include "gtest/gtest.h"
#include "smpp/gsmencoding.h"
#include "smpp/smpp_params.h"
#include "smpp/timeformat.h"

using smpp::encoding::GsmEncoder;
using smpp::SmppAddress;
using smpp::SmppClient;
using smpp::TLV;
using std::list;
using std::string;

//  Test login
TEST_F(SmppClientTest, BindTransmitter) {
  socket_->connect(endpoint_);
  ASSERT_TRUE(socket_->is_open());
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  ASSERT_TRUE(client_->IsBound());
  client_->Unbind();
  socket_->close();
}

TEST_F(SmppClientTest, LoginError) {
  socket_->connect(endpoint_);
  ASSERT_TRUE(socket_->is_open());
  EXPECT_THROW(client_->BindTransmitter("WRONG_USERNAME", FLAGS_password), smpp::InvalidSystemIdException);
  EXPECT_THROW(client_->BindTransmitter(FLAGS_username, "I_FORGET_PASWD"), smpp::SmppException);
}

// Test sending a basic SMS
TEST_F(SmppClientTest, submit) {
  socket_->connect(endpoint_);
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  SmppAddress from("CPPSMPP", smpp::TON::ALPHANUMERIC, smpp::NPI::UNKNOWN);
  SmppAddress to("4513371337", smpp::TON::INTERNATIONAL, smpp::NPI::E164);
  string message = "message to send";
  client_->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  client_->Unbind();
  socket_->close();
}

// Test QUERY_SM feature to pull for a message status
TEST_F(SmppClientTest, querySm) {
  socket_->connect(endpoint_);
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  SmppAddress from("CPPSMPP", smpp::TON::ALPHANUMERIC, smpp::NPI::UNKNOWN);
  SmppAddress to("4513371337", smpp::TON::INTERNATIONAL, smpp::NPI::E164);
  string message = "message to send";
  auto smscResult = client_->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  string smscId = smscResult.first;
  smpp::QuerySmResult result = client_->QuerySm(smscId, from);
  ASSERT_EQ(std::get<0>(result), smscId);
  client_->Unbind();
  socket_->close();
}

// Test the use of TLVs
TEST_F(SmppClientTest, tlv) {
  socket_->connect(endpoint_);
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  SmppAddress from("CPPSMPP", smpp::TON::ALPHANUMERIC, smpp::NPI::UNKNOWN);
  SmppAddress to("4513371337", smpp::TON::INTERNATIONAL, smpp::NPI::E164);
  string message = "message to send";
  struct smpp::SmppParams params;
  list<TLV> taglist;
  taglist.push_back(TLV(smpp::Tag::DEST_ADDR_SUBUNIT,
                        static_cast<uint8_t>(0x01)));  // "flash sms" use-case
  taglist.push_back(TLV(smpp::Tag::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));
  client_->SendSms(from, to, GsmEncoder::EncodeGsm0338(message), params, taglist);
  client_->Unbind();
  socket_->close();
}

// Test combining custom TLVs with CSMS TLVs.
TEST_F(SmppClientTest, tlvExtended) {
  socket_->connect(endpoint_);
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  SmppAddress from("CPPSMPP", smpp::TON::ALPHANUMERIC, smpp::NPI::UNKNOWN);
  SmppAddress to("4513371337", smpp::TON::INTERNATIONAL, smpp::NPI::E164);
  string message;
  message.reserve(170);

  for (int i = 0; i < 14; i++) {
    message += "lorem ipsum ";
  }

  struct smpp::SmppParams params;
  list<TLV> taglist;
  taglist.push_back(TLV(smpp::Tag::DEST_ADDR_SUBUNIT, static_cast<uint8_t>(0x01)));
  taglist.push_back(TLV(smpp::Tag::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));
  int csms_method = client_->csms_method();
  client_->set_csms_method(SmppClient::CSMS_16BIT_TAGS);
  client_->SendSms(from, to, GsmEncoder::EncodeGsm0338(message), params, taglist);
  client_->set_csms_method(csms_method);
  client_->Unbind();
  socket_->close();
}

// Test sending all params to sendSms(). Also sets registered delivery
TEST_F(SmppClientTest, submitExtended) {
  socket_->connect(endpoint_);
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  SmppAddress from(FLAGS_sender, smpp::TON::ALPHANUMERIC, smpp::NPI::UNKNOWN);
  SmppAddress to(FLAGS_receiver, smpp::TON::INTERNATIONAL, smpp::NPI::E164);
  string message = "message to send";

  struct smpp::SmppParams params;
  params.priority_flag = smpp::Priority::GSM_PRIORITY;
  params.registered_delivery = smpp::RegisteredDelivery::DELIVERY_SMSC_BOTH;

  time_t now = time(0);
  struct tm tm;
  localtime_r(&now, &tm);
  params.schedule_delivery_time = smpp::timeformat::ToSmppTimeString(tm);
  params.validity_period = smpp::timeformat::ToSmppTimeString(std::chrono::hours(1));  // valid for one hour
  params.data_coding = smpp::DataCoding::ISO8859_1;

  list<TLV> tags;
  tags.push_back(TLV(smpp::Tag::DEST_ADDR_SUBUNIT,
                        static_cast<uint8_t>(0x01)));  // "flash sms" use-case
  tags.push_back(TLV(smpp::Tag::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));

  client_->SendSms(from, to, GsmEncoder::EncodeGsm0338(message), params, tags);
  client_->Unbind();
  socket_->close();
}

TEST_F(SmppClientTest, logging) {
  FLAGS_v = 1;
  socket_->connect(endpoint_);
  client_->BindTransmitter(FLAGS_username, FLAGS_password);
  client_->Unbind();
  socket_->close();
}

TEST_F(SmppClientCsmsTest, CsmsPayload) {
  client_.set_csms_method(SmppClient::CSMS_PAYLOAD);
  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_170_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_256_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_300_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_400_));
    EXPECT_EQ(3, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_500_));
    EXPECT_EQ(4, r.second);
  }
}

TEST_F(SmppClientCsmsTest, Csms16BitTags) {
  client_.set_csms_method(SmppClient::CSMS_16BIT_TAGS);
  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_170_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_256_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_300_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_400_));
    EXPECT_EQ(3, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_500_));
    EXPECT_EQ(4, r.second);
  }
}

TEST_F(SmppClientCsmsTest, Csms8BitUdh) {
  client_.set_csms_method(SmppClient::CSMS_8BIT_UDH);
  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_170_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_256_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_300_));
    EXPECT_EQ(2, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_400_));
    EXPECT_EQ(3, r.second);
  }

  {
    auto r = client_.SendSms(sender_, receiver_, GsmEncoder::EncodeGsm0338(message_500_));
    EXPECT_EQ(4, r.second);
  }
}
