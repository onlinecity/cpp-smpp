#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdexcept>
/**
 * Extension of the standard exception that can hold a status message.
 */
class OcException: public std::exception {
protected:
	std::string message;

public:
	OcException() :
			message() {
	}
	OcException(const std::string &_message) :
			message(_message) {
	}

	virtual ~OcException() throw () {
	}

	virtual const char* what() const throw () {
		if (message.empty()) {
			return "Default OcException.";
		} else {
			return message.c_str();
		}
	}
};

namespace smpp {
/**
 * Exception thrown when there is SMPP protocol related issues.
 */
class SmppException: public OcException {
public:
	SmppException() :
			OcException("Default SmppException.") {
	}
	SmppException(const std::string &message) :
			OcException(message) {
	}
};

/**
 * Exception thrown when there is transport/connection related issues.
 */
class TransportException: public OcException {
public:
	TransportException() :
			OcException("Default TransportException.") {
	}
	TransportException(const std::string &message) :
			OcException(message) {
	}
};
}

#endif /* EXCEPTIONS_H_ */

