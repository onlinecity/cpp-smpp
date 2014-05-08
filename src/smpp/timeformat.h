// Copyright (C) 2011-2014 OnlineCity
// Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
// @author hd@onlinecity.dk & td@onlinecity.dk

#pragma once

#include <chrono>
#include <regex>
#include <string>
#include <utility>

#include "smpp/exceptions.h"

namespace smpp {
namespace timeformat {

typedef std::pair<std::chrono::time_point<std::chrono::system_clock>,
        std::chrono::seconds> ChronoDatePair;
// Parses a relative timestamp and returns it as a chrono seconds duration.
//
std::chrono::seconds ParseRelativeTimestamp(const std::smatch &match);

// Parses an absolute timestamp and returns it as a time_point.
std::chrono::time_point<std::chrono::system_clock> ParseAbsoluteTimestamp(const std::smatch &match);

// Parses a smpp timestamp and returns a DatePair representation of the timestamp.
// The smpp timestamp has the following format: “YYMMDDhhmmsstnnp”, an SmppException
// thrown if the input timeformat differs.
// @throw SmppException.
ChronoDatePair ParseSmppTimestamp(const std::string &time);

// Parses a delivery receipt timestamp and returns it as time_point.
std::chrono::time_point<std::chrono::system_clock> ParseDlrTimestamp(const std::string &time);

// Returns the struct tm as a string formatted as an absolute timestamp
std::string ToSmppTimeString(const struct tm &tm);

// Returns a relative timestamp created from the time_duration. Since a time_duration does not handle
// dates, the relative dates is calculated with the assumption of one year equals 365 days and a month is 30 days long.
std::string ToSmppTimeString(const std::chrono::seconds &d);

}  // namespace timeformat
}  // namespace smpp
