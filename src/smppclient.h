#ifndef SMPPCLIENT_H_
#define SMPPCLIENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/function.hpp>
#include <boost/bind/bind.hpp>

#include <string>
#include <stdint.h>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>

#include "smpp.h"
#include "tlv.h"
#include "pdu.h"
#include "sms.h"
#include "exceptions.h"
#include "timeformat.h"

namespace smpp {

typedef boost::tuple<std::string, boost::local_time::local_date_time, int, int> QuerySmResult;

/**
 * Class for sending and receiving SMSes through the SMPP protocol.
 * This clients goal is to simplify sending an SMS and receiving
 * delivery reports and therefore not all features of the SMPP protocol is
 * implemented.
 */
class SmppClient
{
private:
	enum
	{
		OPEN, BOUND_TX, BOUND_RX, BOUND_TRX
	};

	// SMPP bind parameters
	std::string systemType;
	uint8_t interfaceVersion; //= 0x34;
	uint8_t addrTon; // = 0;
	uint8_t addrNpi; // = 0;
	std::string addrRange;
	// ESME transmitter parameters
	std::string serviceType;
	uint8_t esmClass;
	uint8_t protocolId;
	uint8_t registeredDelivery;
	uint8_t replaceIfPresentFlag;
	uint8_t smDefaultMsgId;

	// Extra options;
	bool nullTerminateOctetStrings;
	bool useMsgPayload;

	boost::function<uint16_t()> msgRefCallback;

	int state;
	boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
	uint32_t seqNo;
	std::list<PDU> pdu_queue;
	// Socket write timeout in milliseconds. Default is 5000 milliseconds.
	int socketWriteTimeout;
	// Socket read timeout in milliseconds. Default is 30000 milliseconds.
	int socketReadTimeout;

	bool verbose;

public:
	/**
	 * Constructs a new SmppClient object.
	 */
	SmppClient(boost::shared_ptr<boost::asio::ip::tcp::socket> _socket) :
					systemType("WWW"),
					interfaceVersion(0x34),
					addrTon(0),
					addrNpi(0),
					addrRange(""),
					serviceType(""),
					esmClass(0),
					protocolId(0),
					registeredDelivery(0),
					replaceIfPresentFlag(0),
					smDefaultMsgId(0),
					nullTerminateOctetStrings(true),
					useMsgPayload(false),
					msgRefCallback(boost::bind(&SmppClient::defaultMessageRef, this)),
					state(OPEN),
					socket(_socket),
					seqNo(0),
					pdu_queue(),
					socketWriteTimeout(5000),
					socketReadTimeout(30000),
					verbose(false)
	{
	}

	~SmppClient();

	/**
	 * Binds the client in transmitter mode.
	 * @param login SMSC login.
	 * @param password SMS password.
	 */
	void bindTransmitter(const std::string &login, const std::string &password);

	/**
	 * Binds the client in receiver mode.
	 * @param login SMSC login
	 * @param password SMSC password.
	 */
	void bindReceiver(const std::string &login, const std::string &password);

	/**
	 * Unbinds the client.
	 */
	void unbind();

	/**
	 * Sends an SMS without tags to the SMSC.
	 * The SMS is split into multiple if it doesn't into one.
	 *
	 * @param sender
	 * @param receiver
	 * @param shortMessage
	 * @param priority_flag
	 * @param schedule_delivery_time
	 * @param validity_period
	 * @param dataCoding
	 * @return SMSC sms id.
	 */
	std::string
	sendSms(const SmppAddress &sender, const SmppAddress &receiver, const std::string &shortMessage,
			const uint8_t priority_flag = 0, const std::string &schedule_delivery_time = "",
			const std::string &validity_period = "", const int dataCoding = smpp::DATA_CODING_DEFAULT);

	/**
	 * Sends an SMS to the SMSC.
	 * The SMS is split into multiple if it doesn't into one.
	 *
	 * @param sender
	 * @param receiver
	 * @param shortMessage
	 * @param tags
	 * @param priority_flag
	 * @param schedule_delivery_time
	 * @param validity_period
	 * @param dataCoding
	 * @return SMSC sms id.
	 */
	std::string sendSms(const SmppAddress &sender, const SmppAddress &receiver, const std::string &shortMessage,
			std::list<TLV> tags, const uint8_t priority_flag = 0, const std::string &schedule_delivery_time = "",
			const std::string &validity_period = "", const int dataCoding = smpp::DATA_CODING_DEFAULT);
	/**
	 * Returns the first SMS in the PDU queue,
	 * or does a blocking read on the socket until we receive an SMS from the SMSC.
	 */
	smpp::SMS readSms();

	/**
	 * Query the SMSC about current state/status of a previous sent SMS.
	 * You must specify the SMSC assigned message id and source of the sent SMS.
	 * Returns an boost::tuple with elements: message_id, final_date, message_state and error_code.
	 * message_state would be one of the SMPP::STATE_* constants. (SMPP v3.4 section 5.2.28)
	 * error_code depends on the telco network, so could be anything.
	 *
	 * @param messageid
	 * @param source
	 * @return QuerySmResult
	 */
	QuerySmResult querySm(std::string messageid, SmppAddress source);

	/**
	 * Sends an enquire link command to SMSC and blocks until we a response.
	 */
	void enquireLink();

	/**
	 * Checks if the SMSC has sent us a enquire link command.
	 * If they have, a response is sent.
	 */
	void enquireLinkRespond();

	/**
	 * Returns true if the client is bound.
	 * @return
	 */
	bool isBound()
	{
		return state != OPEN;
	}

	void setSystemType(const std::string s)
	{
		systemType = s;
	}

	std::string getSystemType() const
	{
		return systemType;
	}

	void setInterfaceVersion(const uint8_t i)
	{
		interfaceVersion = i;
	}

	uint8_t getInterfaceVersion() const
	{
		return interfaceVersion;
	}

	void setAddrTon(const uint8_t i)
	{
		addrTon = i;
	}

	uint8_t getAddrTon() const
	{
		return addrTon;
	}

	void setAddrNpi(const uint8_t i)
	{
		addrNpi = i;
	}

	uint8_t getAddrNpi() const
	{
		return addrNpi;
	}

	void setAddrRange(const std::string &s)
	{
		addrRange = s;
	}

	std::string getAddrRange() const
	{
		return addrRange;
	}

	void setServiceType(const std::string &s)
	{
		serviceType = s;
	}

	std::string getServiceType() const
	{
		return serviceType;
	}

	void setEsmClass(const uint8_t i)
	{
		esmClass = i;
	}

	uint8_t getEsmClass() const
	{
		return esmClass;
	}

	void setProtocolId(const uint8_t i)
	{
		protocolId = i;
	}

	uint8_t getProtocolId() const
	{
		return protocolId;
	}

	void setRegisteredDelivery(const uint8_t i)
	{
		registeredDelivery = i;
	}

	uint8_t getRegisteredDelivery() const
	{
		return registeredDelivery;
	}

	void setReplaceIfPresentFlag(const uint8_t i)
	{
		replaceIfPresentFlag = i;
	}

	uint8_t getReplaceIfPresentFlag() const
	{
		return replaceIfPresentFlag;
	}

	void setSmDefaultMsgId(const uint8_t i)
	{
		smDefaultMsgId = i;
	}

	uint8_t getSmDefaultMsgId() const
	{
		return smDefaultMsgId;
	}

	void setNullTerminateOctetStrings(const bool b)
	{
		nullTerminateOctetStrings = b;
	}

	bool getNullTerminateOctetStrings() const
	{
		return nullTerminateOctetStrings;
	}

	void setUseMsgPayload(const bool b)
	{
		useMsgPayload = b;
	}

	bool getUseMsgPayload() const
	{
		return useMsgPayload;
	}

	/**
	 * Sets the socket read timeout in milliseconds. Default is 5000 milliseconds.
	 * @param timeout Socket read timeout in milliseconds.
	 */
	void setSocketReadTimeout(const int &timeout)
	{
		socketReadTimeout = timeout;
	}

	/**
	 * Returns the socket read timeout.
	 * @return Socket read timeout in milliseconds.
	 */
	int getSocketReadTimeout() const
	{
		return socketReadTimeout;
	}

	/**
	 * Sets the socket write timeout in milliseconds. Default is 30000 milliseconds.
	 * @param timeout Socket write timeout in milliseconds.
	 */
	void setSocketWriteTimeout(const int &timeout)
	{
		socketWriteTimeout = timeout;
	}

	/**
	 * Returns the socket write timeout in milliseconds.
	 * @return Socket write timeout in milliseconds.
	 */
	int getSocketWriteTimeout() const
	{
		return socketWriteTimeout;
	}

	void setVerbose(const bool b)
	{
		verbose = b;
	}

	bool isVerbose() const
	{
		return verbose;
	}

	/**
	 * Set callback method for generating message references.
	 * The returned integer must be modulo 65535 (0xffff)
	 * @param cb
	 */
	void setMsgRefCallback(boost::function<uint16_t()> cb)
	{
		msgRefCallback = cb;
	}

private:

	/**
	 * Binds the client to be in the mode specified in the mode parameter.
	 * @param mode Mode to bind client in.
	 * @param login SMSC login.
	 * @param password SMSC password.
	 */
	void bind(uint32_t mode, const std::string &login, const std::string &password);

	/**
	 * Constructs a PDU for binding the client.
	 * @param mode Mode to bind client in.
	 * @param login SMSC login.
	 * @param password SMSC password.
	 * @return PDU for binding the client.
	 */
	smpp::PDU setupBindPdu(uint32_t mode, const std::string &login, const std::string &password);

	/**
	 * Runs through the PDU queue and returns the first sms it finds, and sends a reponse to the SMSC.
	 * While running through the PDU queuy, Alert notification and DataSm PDU are handled as well.
	 *
	 * @return First sms found in the PDU queue or a null sms if there was no smses.
	 */
	smpp::SMS parseSms();

	/**
	 * Splits a string, without leaving a dangling escape character, into an vector of substrings of a given length,
	 *
	 * @param shortMessage String to split.
	 * @param split How long each substring should be.
	 * @param dataCoding
	 * @return Vector of substrings.
	 */
	std::vector<std::string> split(const std::string &shortMessage, const int split, const int dataCoding =
			smpp::DATA_CODING_DEFAULT);

	/**
	 * Sends a SUBMIT_SM pdu with the required details for sending an SMS to the SMSC.
	 * It blocks until it gets a response from the SMSC.
	 *
	 * @param sender
	 * @param receiver
	 * @param shortMessage
	 * @param tags
	 * @param priority_flag
	 * @param schedule_delivery_time
	 * @param validity_period
	 * @return SMSC sms id.
	 */
	std::string submitSm(const SmppAddress &sender, const SmppAddress &receiver, const std::string &shortMessage,
			std::list<TLV> tags, const uint8_t priority_flag, const std::string &schedule_delivery_time,
			const std::string& validity_period, const int dataCoding = smpp::DATA_CODING_DEFAULT);

	/**
	 * @return Returns the next sequence number.
	 * @throw SmppException Throws an SmppException if we run out of sequence numbers.
	 */
	uint32_t nextSequenceNumber();

	/**
	 * Sends one PDU to the SMSC.
	 */
	void sendPdu(PDU &pdu);

	/**
	 * Sends one PDU to the SMSC and blocks until we a response to it.
	 * @param pdu PDU to send.
	 * @return PDU PDU response to the one we sent.
	 */
	smpp::PDU sendCommand(PDU &pdu);

	/** Async write callback */
	void writeHandler(const boost::system::error_code&);

	/**
	 * Returns one PDU from SMSC.
	 */
	PDU readPdu(const bool&);

	void readPduBlocking();

	void handleTimeout(boost::optional<boost::system::error_code>* opt, const boost::system::error_code &error);

	/**
	 * Peeks at the socket and returns true if there is data to be read.
	 * @return True if there is data to be read.
	 */
	bool socketPeek();

	/**
	 * Executes any pending async operations on the socket.
	 */
	void socketExecute();

	/**
	 * Handler for reading a PDU header.
	 * If we read a valid PDU header on the socket, the readPduBodyHandler is invoked.
	 * @param error Boost error code
	 * @param read Bytes read
	 */
	void readPduHeaderHandler(const boost::system::error_code &error, size_t read,
			const boost::shared_array<uint8_t> &pduLength);

	void readPduHeaderHandlerBlocking(boost::optional<boost::system::error_code>* opt,
			const boost::system::error_code &error, size_t read, boost::shared_array<uint8_t> pduLength);

	/**
	 * Handler for reading a PDU body.
	 * Reads a PDU body on the socket and pushes it onto the PDU queue.
	 *
	 * @param error Boost error code
	 * @param read Bytes read
	 */
	void readPduBodyHandler(const boost::system::error_code &error, size_t read, boost::shared_array<uint8_t> pduLength,
			boost::shared_array<uint8_t> pduBuffer);

	/**
	 * Returns a response for a PDU we have sent,
	 * specified by its sequence number and its command id.
	 * First the PDU queue is checked for any responses,
	 * if we don't find any we do a blocking read on the socket until
	 * we get the desired response.
	 *
	 * @param sequence Sequence number to look for.
	 * @param commandId Command id to look for.
	 * @return PDU response to PDU with the given sequence number and command id.
	 */
	PDU readPduResponse(const uint32_t &sequence, const uint32_t &commandId);

	/**
	 * Checks the connection.
	 * @throw TransportException if there was an problem with the connection.
	 */
	void checkConnection();

	/**
	 * Checks if the client is in the desired state.
	 * @param state Desired state.
	 * @throw SmppException if the client is not in the desired state.
	 */

	void checkState(const int state);

	/**
	 * Default implementation for msgRefCallback.
	 * Simple initializes a integer on the heap and increments it for each message reference.
	 * Returns a modulo 0xffff int.
	 * @return
	 */
	uint16_t defaultMessageRef()
	{
		static int ref = 0;
		return (ref++ % 0xffff);
	}

};
}

#endif /* SMPPCLIENT_H_ */
