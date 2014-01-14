/*
 * Copyright (C) 2014 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 */

#include <glog/logging.h>
#include <gflags/gflags.h>
#include <string>
#include "gtest/gtest.h"
#include "smpp/gsmencoding.h"

TEST(GsmEncoder, encodeDecode) {
    std::string i1(
            "Ja r i høje @£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉÄÖÑÜ§¿äöñüà^{}\\[~]| Lorem ipsum, Lorem ipsum, Lorem ipsum, Lorem ipsum, Lorem ipsum, Lorem ipsum, ");
    std::string o1 = oc::tools::GsmEncoder::getGsm0338(i1);
    std::string o3 = oc::tools::GsmEncoder::getUtf8(o1);
    ASSERT_EQ(i1, o3);
}

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

