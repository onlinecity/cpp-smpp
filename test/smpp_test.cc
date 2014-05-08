//
// Copyright (C) 2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
//

#include "gtest/gtest.h"
#include "smpp/smpp.h"

TEST(SmppTest, EsmOperator) {
  using smpp::ESM;
  EXPECT_EQ(0x40 | 0x01, ESM::UHDI | ESM::SUBMIT_MODE_DATAGRAM);
  EXPECT_EQ(0x40 | 0x04, ESM::UHDI | ESM::SUBMIT_BINARY);

  EXPECT_EQ(0x40 & 0x01, ESM::UHDI & ESM::SUBMIT_MODE_DATAGRAM);
  EXPECT_EQ(0x40 & 0x04, ESM::UHDI & ESM::SUBMIT_BINARY);
}
