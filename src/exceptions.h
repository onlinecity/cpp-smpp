#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_
#include <stdexcept>

namespace smpp {
/**
 * Exception thrown when there is SMPP protocol related issues.
 */
class SmppException: public std::runtime_error
{
public:
	SmppException() :
			std::runtime_error("Default SmppException.")
	{
	}
	SmppException(const std::string &message) :
			std::runtime_error(message)
	{
	}
};

/**
 * Exception thrown when there is transport/connection related issues.
 */
class TransportException: public std::runtime_error
{
public:
	TransportException() :
			std::runtime_error("Default TransportException.")
	{
	}
	TransportException(const std::string &message) :
			std::runtime_error(message)
	{
	}
};
}

#endif /* EXCEPTIONS_H_ */

