/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef SMPP_LOG_H_
#define SMPP_LOG_H_

#include <iostream>
#include "smpp/pdu.h"

/**
 * Simple wrapper for logging to standard output (std::cout).
 * Override the << operator for custom logging.
 */
namespace smpp {
class SmppLog {
public:
    virtual SmppLog & operator<<(PDU &pdu) {
        std::cout << pdu;
        return *this;
    }
    virtual ~SmppLog() {
    }
};

}  // namespace smpp

#endif  // SMPP_LOG_H_
