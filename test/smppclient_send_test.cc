//
// Copyright (C) 2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
//

#include <gflags/gflags.h>
#include <glog/logging.h>
#include "./smppclient_test.h"

using std::string;

std::string HexString(const unsigned char *input, const size_t len ) {
  static const char* const lut = "0123456789abcdef";
  std::string output;
  output.reserve(2 * len);
  for (size_t i = 0; i < len; ++i) {
    const unsigned char c = input[i];
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }
  return output;
}

TEST_F(SmppClientCsmsTest, sms) {
  string message = "Hello world";
  client_.SendSms(sender_, receiver_, message);
}

TEST_F(SmppClientCsmsTest, binary) {
  char s = 0x06;
  uint8_t udh[] = { 0x05, 0x04, 0x0b, 0x84, 0x23, 0xf0 };
  uint8_t payload[] = {
    0x55, 0x06, 0x01, 0xae, 0x02, 0x05, 0x6a, 0x00,
    0x45, 0xc6, 0x0c, 0x03, 0x77, 0x61, 0x70, 0x2e,
    0x79, 0x61, 0x68, 0x6f, 0x6f, 0x2e, 0x63, 0x6f,
    0x6d, 0x2f, 0x00, 0x11, 0x03, 0x36, 0x35, 0x33,
    0x32, 0x00, 0x07, 0x01, 0x03, 0x4c, 0x6f, 0x72,
    0x65, 0x6d, 0x20, 0x69, 0x70, 0x73, 0x75, 0x6d,
    0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x73,
    0x69, 0x74, 0x20, 0x61, 0x6d, 0x65, 0x74, 0x00,
    0x01, 0x01
  };

  string message = s + string(reinterpret_cast<char*>(udh), 6) +
    string(reinterpret_cast<char*>(payload), 66);
  smpp::SmppParams params;
  params.esm_class = smpp::ESM(smpp::ESM::SUBMIT_BINARY | smpp::ESM::UHDI);
  params.data_coding = smpp::DataCoding::BINARY;
  client_.SendSms(sender_, receiver_, message, params);
}

TEST_F(SmppClientCsmsTest, WapPush) {
  char s = 0x06;
  char udh[] = "\x05\x04\x0B\x84\x23\xF0"; // Set 16-bit app ports: 0B84 <- 23F0
  size_t udh_size = 6;

  char wsp[] = "\x55\x06\x01\xAE"; // Wireless Session Protocol header
  size_t wsp_size = 4;

  char xml[] = "\x02\x05\x6A\x00\x45\xC6\x0C\x03"
    "oc.dk"         // wapurl
    "\x00\x11\x03\x36\x35\x33\x32\x00\x07\x01\x03"
    "OnlineCity"   // message
    "\x00\x01\x01";
  size_t xml_size = 37;

  string payload = string(wsp, wsp_size) + string(xml, xml_size);
  string message = s + string(udh, udh_size) + payload;

  smpp::SmppParams params;
  params.esm_class = smpp::ESM(smpp::ESM::SUBMIT_BINARY | smpp::ESM::UHDI);
  params.data_coding = smpp::DataCoding::BINARY;

  client_.SendSms(sender_, receiver_, message, params);
}
