#include <iostream>
#include <sstream>

#include "smpp.h"
#include "smppclient.h"
#include "pdu.h"

using namespace smpp;

smpp::SmppClient::~SmppClient()
{
	if (state != OPEN) unbind();
}

void SmppClient::bindTransmitter(const string &login, const string &pass) throw (smpp::SmppException,
		smpp::TransportException)
{

	bind(smpp::BIND_TRANSMITTER, login, pass);
}

void SmppClient::bindReceiver(const string &login, const string &pass) throw (smpp::SmppException,
		smpp::TransportException)
{
	bind(smpp::BIND_RECEIVER, login, pass);
}

void SmppClient::bind(uint32_t mode, const string &login, const string &password) throw (smpp::SmppException,
		smpp::TransportException)
{
	checkConnection();
	checkState(OPEN);

	cout << "bind" << endl;
	PDU pdu = setupBindPdu(mode, login, password);
	sendCommand(pdu);

	switch (mode) {
		case smpp::BIND_RECEIVER:
			state = BOUND_RX;
			break;
		case smpp::BIND_TRANSMITTER:
			state = BOUND_TX;
			break;
	}
}

smpp::PDU SmppClient::setupBindPdu(uint32_t mode, const string &login, const string &password)
{
	PDU pdu(mode, 0, nextSequenceNumber());
	pdu << login;
	pdu << password;
	pdu << system_type;
	pdu << interfaceVersion;
	pdu << addr_ton;
	pdu << addr_npi;
	pdu << addr_range;
	return pdu;
}

void SmppClient::unbind() throw (smpp::SmppException, smpp::TransportException)
{
	checkConnection();
	PDU pdu(smpp::UNBIND, 0, nextSequenceNumber());
	PDU resp = sendCommand(pdu);

	uint32_t pduStatus = resp.getCommandStatus();

	if (pduStatus != smpp::ESME_ROK) throw smpp::SmppException(smpp::getEsmeStatus(pduStatus));

	state = OPEN;
}

void hexDump(const string &s)
{
	for (unsigned int i = 0 ; i < s.size() ; i++) {
		cout << setw(2) << setfill('0') << hex << (int) s[i] << " ";
	}
	cout << endl << dec;
}

/**
 * Send an sms to the smsc.
 */
string SmppClient::sendSms(const SmppAddress& sender, const SmppAddress& receiver, const string& shortMessage,
		const uint8_t priority_flag, const string& schedule_delivery_time, const string& validity_period,
		const int dataCoding) throw (smpp::SmppException, smpp::TransportException)
{

	return sendSms(sender, receiver, shortMessage, list<TLV>(), priority_flag, schedule_delivery_time, validity_period,
			dataCoding);
}

/**
 * Send an sms to the smsc.
 */
string SmppClient::sendSms(const SmppAddress& sender, const SmppAddress& receiver, const string& shortMessage,
		list<TLV> tags, const uint8_t priority_flag, const string& schedule_delivery_time, const string& validity_period,
		const int dataCoding) throw (smpp::SmppException, smpp::TransportException)
{

	int messageLen = shortMessage.length();
	int singleSmsOctetLimit = 254; // Default SMPP standard
	int csmsSplit = -1; // where to split

	switch (dataCoding) {
		case smpp::DATA_CODING_UCS2:
			singleSmsOctetLimit = 140;
			csmsSplit = 132;
			break;
		case smpp::DATA_CODING_DEFAULT:
			singleSmsOctetLimit = 160;
			csmsSplit = 152;
			break;
	}

	// submit_sm if the short message could fit into one pdu.
	if (messageLen <= singleSmsOctetLimit) return submitSm(sender, receiver, shortMessage, tags, priority_flag,
			schedule_delivery_time, validity_period);

	// split message
	vector<string> parts = split(shortMessage, csmsSplit, dataCoding);
	vector<string>::iterator itr = parts.begin();

	tags.push_back(TLV(smpp::tags::SAR_MSG_REF_NUM, (uint16_t) 0x1234));
	tags.push_back(TLV(smpp::tags::SAR_TOTAL_SEGMENTS, (uint8_t) parts.size()));
	int segment = 0;

	string smsId;

	for (; itr < parts.end() ; itr++) {
		tags.push_back(TLV(smpp::tags::SAR_SEGMENT_SEQNUM, ++segment));
		smsId = submitSm(sender, receiver, (*itr), tags, priority_flag, schedule_delivery_time, validity_period);
		// pop SAR_SEGMENT_SEQNUM tag
		tags.pop_back();
	}

	// pop SAR_TOTAL_SEGMENTS tag
	tags.pop_back();
	// pop SAR_MSG_REF_NUM tag
	tags.pop_back();

	return smsId;
}

smpp::SMS SmppClient::readSms() throw (smpp::SmppException, smpp::TransportException)
{
	// see if we're bound correct.
	checkState(BOUND_RX);

	// if  there are any messages in the queue pop the first usable one off and return it
	if (!pdu_queue.empty()) return parseSms();

	// fill queue until we get a DELIVER_SM command
	try {
		bool b = false;

		while (!b) {
			PDU pdu = readPdu(true);
			if (pdu.getCommandId() == ENQUIRE_LINK) {
				PDU resp = PDU(ENQUIRE_LINK_RESP, 0, pdu.getSequenceNo());
				sendPdu(resp);
				continue;
			}

			if (pdu.null) {
				break;
			}

			if (!pdu.null) pdu_queue.push_back(pdu); // save pdu for reading later
			b = pdu.getCommandId() == DELIVER_SM;
		}
	} catch (std::exception &e) {
		throw smpp::TransportException(e.what());
	}

	cout << "parse sms" << endl;

	return parseSms();
}

void SmppClient::enquireLink() throw (smpp::SmppException, smpp::TransportException)
{
	PDU pdu = PDU(ENQUIRE_LINK, 0, nextSequenceNumber());
	sendCommand(pdu);
}

smpp::SMS SmppClient::parseSms()
{
	if (pdu_queue.empty()) return SMS();

	list<PDU>::iterator it = pdu_queue.begin();
	while (it != pdu_queue.end()) {
		if ((*it).getCommandId() == DELIVER_SM) {
			SMS sms(*it);

			// send response to smsc
			PDU resp = PDU(DELIVER_SM_RESP, 0x0, (*it).getSequenceNo());
			resp << 0x0;
			sendPdu(resp);
			// remove sms from queue
			pdu_queue.erase(it);
			return sms;
		}

		if ((*it).getCommandId() == ALERT_NOTIFICATION) {
			it = pdu_queue.erase(it);
			continue;
		}

		if ((*it).getCommandId() == DATA_SM) {
			PDU resp = PDU(DATA_SM_RESP, 0x0, (*it).getSequenceNo());
			resp << 0x0;
			sendPdu(resp);
			it = pdu_queue.erase(it);
			continue;
		}
		++it;
	}

	return SMS();
}

vector<string> SmppClient::split(const string& shortMessage, const int split, const int dataCoding)
{
	vector<string> parts;
	int len = shortMessage.length();
	int pos = 0;
	int n = split;

	while (pos < len) {
		if ((int) shortMessage[pos + n - 1] == 0x1b) // do not split at escape char
		n--;

		parts.push_back(shortMessage.substr(pos, n));

		pos += n;
		n = split;

		if (pos + n > len) n = len - pos;
	}
	return parts;
}

string SmppClient::submitSm(const SmppAddress& sender, const SmppAddress& receiver, const string& shortMessage,
		list<TLV> tags, const uint8_t priority_flag, const string& schedule_delivery_time, const string& validity_period)
				throw (smpp::SmppException, smpp::TransportException)
{

	checkState(BOUND_TX);

	PDU pdu(smpp::SUBMIT_SM, 0, nextSequenceNumber());
	pdu << service_type;

	pdu << sender;
	pdu << receiver;

	pdu << esmClass;
	pdu << protocol_id;

	pdu << priority_flag;
	pdu << schedule_delivery_time;
	pdu << validity_period;

	pdu << registered_delivery;
	pdu << replace_if_present_flag;
	pdu << data_coding;
	pdu << sm_default_msg_id;

	if (useMsgPayload) {
		pdu << 0; // sm_length = 0
		pdu << TLV(smpp::tags::MESSAGE_PAYLOAD, shortMessage);
	} else {
		pdu.setNullTerminateOctetStrings(nullTerminateOctetStrings);
		pdu << (int) shortMessage.length() + (nullTerminateOctetStrings ? 1 : 0);
		pdu << shortMessage;
		pdu.setNullTerminateOctetStrings(true);
	}

	// add  optional tags.
	for (list<TLV>::iterator itr = tags.begin() ; itr != tags.end() ; itr++)
		pdu << *itr;

	PDU resp = sendCommand(pdu);
	string messageid;
	resp >> messageid;
	return messageid;
}

uint32_t SmppClient::nextSequenceNumber() throw (smpp::SmppException)
{
	if (++seqNo > 0x7FFFFFFF) throw SmppException("Ran out of sequence numbers");

	return seqNo;
}

void SmppClient::sendPdu(PDU &pdu) throw (smpp::SmppException, smpp::TransportException)
{
	checkConnection();

	boost::optional<boost::system::error_code> ioResult;
	boost::optional<boost::system::error_code> timerResult;

	if (verbose) {
		cout << "===>" << endl;
		std::cout << pdu << endl;
	}

	deadline_timer timer(socket->io_service());
	timer.expires_from_now(boost::posix_time::seconds(5));
	timer.async_wait(boost::bind(&SmppClient::handleTimeout, this, &timerResult, _1));

	async_write(*socket, buffer(pdu.getOctets().get(), pdu.getSize()),
			boost::bind(&SmppClient::handleTimeout, this, &ioResult, _1));

	socketExecute();

	if (ioResult) {
		timer.cancel();
	} else if (timerResult) {
		socket->cancel();
	}

	socketExecute();
}

smpp::PDU SmppClient::sendCommand(PDU &pdu) throw (smpp::SmppException, smpp::TransportException)
{
	sendPdu(pdu);

	PDU resp = readPduResponse(pdu.getSequenceNo(), pdu.getCommandId());

	if (resp.getCommandStatus() != smpp::ESME_ROK) throw smpp::SmppException(
			smpp::getEsmeStatus(resp.getCommandStatus()));

	return resp;
}

void SmppClient::writeHandler(const boost::system::error_code &error)
{
}

smpp::PDU SmppClient::readPdu(const bool &isBlocking) throw (smpp::SmppException, smpp::TransportException)
{

	// return NULL pdu if there is nothing on the wire for us.
	if (!isBlocking && !socketPeek()) return PDU();

	readPduBlocking();

	// There are no pdus to be read return a null pdu.
	if (pdu_queue.empty()) return PDU();

	// Return last the pdu inserted into the queue.
	PDU pdu = pdu_queue.back();
	pdu_queue.pop_back();

	if (verbose) {
		cout << "<===" << endl;
		cout << pdu;
	}

	return pdu;
}

bool SmppClient::socketPeek()
{
	// prepare our read
	shared_array<uint8_t> pduHeader(new uint8_t[4]);
	async_read(*socket, buffer(pduHeader.get(), 4),
			boost::bind(&smpp::SmppClient::readPduHeaderHandler, this, _1, _2, pduHeader));

	size_t handlersCalled = socket->get_io_service().poll_one();

	socket->get_io_service().reset();
	socket->cancel();

	socketExecute();

	return handlersCalled != 0;
}

void SmppClient::readPduBlocking()
{

	using namespace boost;
	boost::optional<boost::system::error_code> ioResult;
	boost::optional<boost::system::error_code> timerResult;

	shared_array<uint8_t> pduHeader(new uint8_t[4]);

	async_read(*socket, boost::asio::buffer(pduHeader.get(), 4),
			boost::bind(&SmppClient::readPduHeaderHandlerBlocking, this, &ioResult, _1, _2, pduHeader));

	deadline_timer timer(socket->io_service());
	timer.expires_from_now(boost::posix_time::seconds(30));
	timer.async_wait(boost::bind(&SmppClient::handleTimeout, this, &timerResult, _1));

	socketExecute();

	if (ioResult) {
		timer.cancel();
	} else if (timerResult) {
		socket->cancel();
	}

	socketExecute();
}

void SmppClient::handleTimeout(boost::optional<boost::system::error_code>* opt, const boost::system::error_code &error)
{
	opt->reset(error);
}

void SmppClient::socketExecute()
{
	socket->get_io_service().run_one();
	socket->get_io_service().reset();
}

void SmppClient::readPduHeaderHandler(const boost::system::error_code &error, size_t len,
		const shared_array<uint8_t> &pduLength) throw (smpp::TransportException)
{
	if (error) {

		if (error == boost::asio::error::operation_aborted) {
			// Not treated as an error
			return;
		}
		throw TransportException(boost::system::system_error(error).what());
	}

	unsigned int i = (int) pduLength[0] << 24 | (int) pduLength[1] << 16 | (int) pduLength[2] << 8 | (int) pduLength[3];
	shared_array<uint8_t> pduBuffer(new uint8_t[i]);

	// start reading after the size mark of the pdu
	async_read(*socket, buffer(pduBuffer.get(), i - 4),
			boost::bind(&smpp::SmppClient::readPduBodyHandler, this, _1, _2, pduLength, pduBuffer));

	socketExecute();
}

void SmppClient::readPduHeaderHandlerBlocking(boost::optional<boost::system::error_code>* opt,
		const boost::system::error_code &error, size_t read, shared_array<uint8_t> pduLength)
				throw (smpp::TransportException)
{

	if (error) {

		if (error == boost::asio::error::operation_aborted) {
			// Not treated as an error
			return;
		}
		throw TransportException(boost::system::system_error(error).what());
	}

	opt->reset(error);

	unsigned int i = (int) pduLength[0] << 24 | (int) pduLength[1] << 16 | (int) pduLength[2] << 8 | (int) pduLength[3];
	shared_array<uint8_t> pduBuffer(new uint8_t[i - 4]);

	// start reading after the size mark of the pdu
async_read	(*socket, buffer(pduBuffer.get(), i - 4),
			boost::bind(&smpp::SmppClient::readPduBodyHandler, this, _1, _2, pduLength, pduBuffer));

	socketExecute();
}

void SmppClient::readPduBodyHandler(const boost::system::error_code &error, size_t len, shared_array<uint8_t> pduLength,
		shared_array<uint8_t> pduBuffer)
{
	if (error) {
		throw TransportException(boost::system::system_error(error).what());
	}

	PDU pdu(pduLength, pduBuffer);

	cout << "read pdu body handler:" << endl;
	cout << pdu;

	pdu_queue.push_back(pdu);

}

// blocks until response is read
smpp::PDU smpp::SmppClient::readPduResponse(const uint32_t &sequence, const uint32_t &commandId)
		throw (smpp::SmppException, smpp::TransportException)
{
	uint32_t response = GENERIC_NACK | commandId;
	list<PDU>::iterator it = pdu_queue.begin();

	while (it != pdu_queue.end()) {
		PDU pdu = (*it);

		if (pdu.getSequenceNo() == sequence && pdu.getCommandId() == response) {
			it = pdu_queue.erase(it);
			return pdu;
		}
		it++;
	}

	while (true) {
		PDU pdu = readPdu(true);
		if (!pdu.null) {
			if (pdu.getSequenceNo() == sequence && pdu.getCommandId() == response) return pdu;
		}
	}

	PDU pdu;
	return pdu;
}

void smpp::SmppClient::enquireLinkRespond() throw (smpp::SmppException, smpp::TransportException)
{
	list<PDU>::iterator it = pdu_queue.begin();

	while (it != pdu_queue.end()) {
		PDU pdu = (*it);
		if (pdu.getCommandId() == ENQUIRE_LINK) {
			PDU resp = PDU(ENQUIRE_LINK_RESP, 0, pdu.getSequenceNo());
			sendCommand(resp);
		}
		it++;
	}

	PDU pdu = readPdu(false);

	if (!pdu.null && pdu.getCommandId() == ENQUIRE_LINK) {
		PDU resp = PDU(ENQUIRE_LINK_RESP, 0, pdu.getSequenceNo());
		sendPdu(resp);
	}
}

uint32_t smpp::SmppClient::byteRead(uint8_t* bytes)
{
	uint32_t r = 0;

	for (int i = 0 ; i < 4 ; i++) {
		r <<= 8;
		r |= (uint32_t) bytes[i];
	}

	return r;
}

void smpp::SmppClient::checkConnection() throw (smpp::TransportException)
{
	if (!socket->is_open()) throw smpp::TransportException("Socket is closed");
}

void smpp::SmppClient::checkState(int state) throw (smpp::SmppException)
{
	if (this->state != state) throw smpp::SmppException("Client in wrong state");
}
