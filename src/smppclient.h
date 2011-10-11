#ifndef SMPPCLIENT_H_
#define SMPPCLIENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <string>
#include <stdint.h>
#include <vector>
#include <list>
#include <iostream>

#include "smpp.h"
#include "tlv.h"
#include "pdu.h"
#include "sms.h"
#include "exceptions.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using boost::asio::ip::tcp;

namespace smpp {
/**
 * Class for sending and receiving SMSes through the SMPP protocol.
 * This clients goal is to simplify sending an SMS and receiving
 * delivery reports and therefore not all features of the SMPP protocol is
 * implemented.
 */
class SmppClient
{
public:
	enum
	{
		OPEN, BOUND_TX, BOUND_RX, BOUND_TRX
	};

	// SMPP bind parameters
	string system_type;
	uint8_t interfaceVersion; //= 0x34;
	uint8_t addr_ton; // = 0;
	uint8_t addr_npi; // = 0;
	string addr_range;

	// ESME transmitter parameters
	string service_type;
	uint8_t esmClass;
	uint8_t protocol_id;
	uint8_t registered_delivery;
	uint8_t replace_if_present_flag;
	uint8_t data_coding;
	uint8_t sm_default_msg_id;

	// Extra options;
	bool nullTerminateOctetStrings;
	bool useMsgPayload;

private:
	int state;
	shared_ptr<tcp::socket> socket;
	uint32_t seqNo;
	list<PDU> pdu_queue;

public:
	bool verbose;

public:
	/**
	 * Constructs a new SmppClient object.
	 */
	SmppClient(shared_ptr<tcp::socket> _socket) :
					system_type("WWW"),
					interfaceVersion(0x34),
					addr_ton(0),
					addr_npi(0),
					addr_range(""),
					service_type(""),
					esmClass(0),
					protocol_id(0),
					registered_delivery(0),
					replace_if_present_flag(0),
					data_coding(0),
					sm_default_msg_id(0),
					nullTerminateOctetStrings(false),
					useMsgPayload(false),
					state(OPEN),
					socket(_socket),
					seqNo(0),
					pdu_queue(),
					verbose(false)
	{
	}

	~SmppClient();

	/**
	 * Binds the client in transmitter mode.
	 * @param login SMSC login.
	 * @param password SMS password.
	 */
	void bindTransmitter(const string &login, const string &password) throw (smpp::SmppException,
			smpp::TransportException);

	/**
	 * Binds the client in receiver mode.
	 * @param login SMSC login
	 * @param password SMSC password.
	 */
	void bindReceiver(const string &login, const string &password) throw (smpp::SmppException, smpp::TransportException);

	/**
	 * Unbinds the client.
	 */
	void unbind() throw (smpp::SmppException, smpp::TransportException);

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
	string
	sendSms(const SmppAddress &sender, const SmppAddress &receiver, const string &shortMessage,
			const uint8_t priority_flag = 0, const string &schedule_delivery_time = "", const string &validity_period = "",
			const int dataCoding = smpp::DATA_CODING_DEFAULT) throw (smpp::SmppException, smpp::TransportException);

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
	string sendSms(const SmppAddress &sender, const SmppAddress &receiver, const string &shortMessage, list<TLV> tags,
			const uint8_t priority_flag = 0, const string &schedule_delivery_time = "", const string &validity_period = "",
			const int dataCoding = smpp::DATA_CODING_DEFAULT) throw (smpp::SmppException, smpp::TransportException);

	/**
	 * Returns the first SMS in the PDU queue,
	 * or does a blocking read on the socket until we receive an SMS from the SMSC.
	 */
	smpp::SMS readSms() throw (smpp::SmppException, smpp::TransportException);

	/**
	 * Sends an enquire link command to SMSC and blocks until we a response.
	 */
	void enquireLink() throw (smpp::SmppException, smpp::TransportException);

	/**
	 * Checks if the SMSC has sent us a enquire link command.
	 * If they have, a response is sent.
	 */
	void enquireLinkRespond() throw (smpp::SmppException, smpp::TransportException);

	/**
	 * Sleeps the given number of milliseconds.
	 * @param milliseconds Milliseconds to sleep.
	 */
//	inline void sleep(const int &milliseconds)
//	{
//		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));
//	}

	inline bool isBound()
	{
		return state != OPEN;
	}

private:

	/**
	 * Binds the client to be in the mode specified in the mode parameter.
	 * @param mode Mode to bind client in.
	 * @param login SMSC login.
	 * @param password SMSC password.
	 */
	void bind(uint32_t mode, const string &login, const string &password) throw (smpp::SmppException,
			smpp::TransportException);

	/**
	 * Constructs a PDU for binding the client.
	 * @param mode Mode to bind client in.
	 * @param login SMSC login.
	 * @param password SMSC password.
	 * @return PDU for binding the client.
	 */
	smpp::PDU setupBindPdu(uint32_t mode, const string &login, const string &password);

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
	vector<string> split(const string &shortMessage, const int split, const int dataCoding = smpp::DATA_CODING_DEFAULT);

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
	string submitSm(const SmppAddress &sender, const SmppAddress &receiver, const string &shortMessage, list<TLV> tags,
			const uint8_t priority_flag, const string &schedule_delivery_time, const string& validity_period)
					throw (smpp::SmppException, smpp::TransportException);

	/**
	 * @return Returns the next sequence number.
	 * @throw SmppException Throws an SmppException if we run out of sequence numbers.
	 */
	uint32_t nextSequenceNumber() throw (smpp::SmppException);

	/**
	 * Sends one PDU to the SMSC.
	 */
	void sendPdu(PDU &pdu) throw (smpp::SmppException, smpp::TransportException);

	/**
	 * Sends one PDU to the SMSC and blocks until we a response to it.
	 * @param pdu PDU to send.
	 * @return PDU PDU response to the one we sent.
	 */
	smpp::PDU sendCommand(PDU &pdu) throw (smpp::SmppException, smpp::TransportException);

	/** Async write callback */
	void writeHandler(const boost::system::error_code&);

	/**
	 * Returns one PDU from SMSC.
	 */
	PDU readPdu(const bool&) throw (smpp::SmppException, smpp::TransportException);

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
	void readPduHeaderHandler(const boost::system::error_code &error, size_t read, const shared_array<uint8_t> &pduLength)
			throw (smpp::TransportException);

	void readPduHeaderHandlerBlocking(boost::optional<boost::system::error_code>* opt,
			const boost::system::error_code &error, size_t read, shared_array<uint8_t> pduLength)
					throw (smpp::TransportException);

	/**
	 * Handler for reading a PDU body.
	 * Reads a PDU body on the socket and pushes it onto the PDU queue.
	 *
	 * @param error Boost error code
	 * @param read Bytes read
	 */
	void readPduBodyHandler(const boost::system::error_code &error, size_t read, shared_array<uint8_t> pduLength,
			shared_array<uint8_t> pduBuffer);

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
	PDU readPduResponse(const uint32_t &sequence, const uint32_t &commandId) throw (smpp::SmppException,
			smpp::TransportException);

	/**
	 * Returns an array of four bytes as one uint32_t.
	 * NOTE: This function reads four memory locations no matter what.
	 *
	 * @param bytes Array of bytes to be returned as one uint32_t
	 * @return bytes as uint32_t.
	 */
	uint32_t byteRead(uint8_t* bytes);

	/**
	 * Checks the connection.
	 * @throw TransportException if there was an problem with the connection.
	 */
	void checkConnection() throw (smpp::TransportException);

	/**
	 * Checks if the client is in the desired state.
	 * @param state Desired state.
	 * @throw SmppException if the client is not in the desired state.
	 */

	void checkState(int state) throw (smpp::SmppException);

};
}

#endif /* SMPPCLIENT_H_ */
