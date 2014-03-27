//
// Copyright (C) 2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
//
#ifndef SMPPCLIENT_TEST_H_
#define SMPPCLIENT_TEST_H_
#include "asio.hpp"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <memory>
#include "gtest/gtest.h"
#include "connectionsetting.h"
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
        client->setVerbose(false);
    }

    virtual void TearDown() {
        if (client->isBound()) {
            client->unbind();
        }
        socket->close();
    }
};
#endif  // SMPPCLIENT_TEST_H_
