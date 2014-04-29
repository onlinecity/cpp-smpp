/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 */
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <list>
#include <string>
#include <tuple>
#include "asio.hpp"
#include "gtest/gtest.h"
#include "smpp/gsmencoding.h"
#include "smpp/timeformat.h"
#include "smppclient_test.h"

using oc::tools::GsmEncoder;
using smpp::SmppAddress;
using smpp::SmppClient;
using smpp::TLV;
//using smpp::timeformat::getTimeString;
using std::list;
using std::string;

//  Test login of either transmitter or receiver
TEST_F(SmppClientTest, login) {
  socket->connect(endpoint);
  ASSERT_TRUE(socket->is_open());
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  ASSERT_TRUE(client->IsBound());
  client->Unbind();
  socket->close();
  socket->connect(endpoint);
  ASSERT_TRUE(socket->is_open());
  client->BindReceiver(SMPP_USERNAME, SMPP_PASSWORD);
  ASSERT_TRUE(client->IsBound());
  client->Unbind();
  socket->close();
}

// Test sending a basic SMS
TEST_F(SmppClientTest, submit) {
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  //      client->setNullTerminateOctetStrings(false);
  //      client->setRegisteredDelivery(smpp::REG_DELIVERY_SMSC_BOTH);
  SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
  string message = "message to send";
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  client->Unbind();
  socket->close();
}

// Test QUERY_SM feature to pull for a message status
TEST_F(SmppClientTest, querySm) {
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
  string message = "message to send";
  auto smscResult = client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  string smscId = smscResult.first;
  smpp::QuerySmResult result = client->QuerySm(smscId, from);
  ASSERT_EQ(std::get<0>(result), smscId);
  client->Unbind();
  socket->close();
}

// Test concatenated SMS using either split or payload method.
TEST_F(SmppClientTest, csms) {
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
  // Construct 168 char message
  string message;
  message.reserve(170);

  for (int i = 0; i < 14; i++) {
    message += "lorem ipsum ";
  }

  int csmsMethod = client->getCsmsMethod();
  client->setCsmsMethod(SmppClient::CSMS_PAYLOAD);
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  client->setCsmsMethod(SmppClient::CSMS_16BIT_TAGS);
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  client->setCsmsMethod(SmppClient::CSMS_8BIT_UDH);
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message));
  client->setCsmsMethod(csmsMethod);
  client->Unbind();
  socket->close();
}

// Test the use of TLVs
TEST_F(SmppClientTest, tlv) {
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
  string message = "message to send";
  list<TLV> taglist;
  taglist.push_back(TLV(smpp::tags::DEST_ADDR_SUBUNIT,
                        static_cast<uint8_t>(0x01)));  // "flash sms" use-case
  taglist.push_back(TLV(smpp::tags::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message), taglist);
  client->Unbind();
  socket->close();
}

// Test combining custom TLVs with CSMS TLVs.
TEST_F(SmppClientTest, tlvExtended) {
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
  string message;
  message.reserve(170);

  for (int i = 0; i < 14; i++) {
    message += "lorem ipsum ";
  }

  list<TLV> taglist;
  taglist.push_back(TLV(smpp::tags::DEST_ADDR_SUBUNIT, static_cast<uint8_t>(0x01)));
  taglist.push_back(TLV(smpp::tags::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));
  int csmsMethod = client->getCsmsMethod();
  client->setCsmsMethod(SmppClient::CSMS_16BIT_TAGS);
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message), taglist);
  client->setCsmsMethod(csmsMethod);
  client->Unbind();
  socket->close();
}

// Test sending all params to sendSms(). Also sets registered delivery
TEST_F(SmppClientTest, submitExtended) {
  using boost::posix_time::time_duration;
  using boost::local_time::time_zone_ptr;
  using boost::local_time::posix_time_zone;
  using boost::local_time::local_date_time;
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  SmppAddress from("CPPSMPP", smpp::TON_ALPHANUMERIC, smpp::NPI_UNKNOWN);
  SmppAddress to("4513371337", smpp::TON_INTERNATIONAL, smpp::NPI_E164);
  string message = "message to send";
  list<TLV> taglist;
  taglist.push_back(TLV(smpp::tags::DEST_ADDR_SUBUNIT,
                        static_cast<uint8_t>(0x01)));  // "flash sms" use-case
  taglist.push_back(TLV(smpp::tags::USER_MESSAGE_REFERENCE, static_cast<uint16_t>(0x1337)));

  time_t now = time(0);
  struct tm *tm = localtime(&now);
  string sdt = smpp::timeformat::ToSmppTimeString(*tm);
  string vt = smpp::timeformat::ToSmppTimeString(std::chrono::hours(1));  // valid for one hour
  uint8_t regdlr = client->getRegisteredDelivery();
  client->setRegisteredDelivery(smpp::REG_DELIVERY_SMSC_BOTH);
  client->SendSms(from, to, GsmEncoder::EncodeGsm0338(message), taglist, 0x01, sdt, vt,
                  smpp::DATA_CODING_ISO8859_1);
  client->setRegisteredDelivery(regdlr);
  client->Unbind();
  socket->close();
}

TEST_F(SmppClientTest, receive) {
  socket->connect(endpoint);
  client->BindReceiver(SMPP_USERNAME, SMPP_PASSWORD);
  LOG(INFO) << "Waiting for smpp connection to send message";
  smpp::SMS sms = client->ReadSms();
}

TEST_F(SmppClientTest, logging) {
  client->setVerbose(true);
  socket->connect(endpoint);
  client->BindTransmitter(SMPP_USERNAME, SMPP_PASSWORD);
  client->Unbind();
  socket->close();
}

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
