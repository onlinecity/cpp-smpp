/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "smpp/smppclient.h"
#include <algorithm>
#include <list>
#include <string>
#include <vector>
#include <utility>

using std::string;
using std::vector;
using std::list;
using std::pair;
using std::shared_ptr;

using boost::optional;
using boost::system::system_error;
using boost::system::error_code;
using boost::shared_array;
using boost::numeric_cast;
using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using boost::asio::async_write;
using boost::asio::buffer;
using boost::local_time::local_date_time;
using boost::local_time::not_a_date_time;

namespace smpp {
SmppClient::SmppClient(shared_ptr<tcp::socket> _socket) :
    systemType("WWW"), /**/
    interfaceVersion(0x34), /**/
    addrTon(0), /**/
    addrNpi(0), /**/
    addrRange(""), /**/
    serviceType(""), /**/
    esmClass(0), /**/
    protocolId(0), /**/
    registeredDelivery(0), /**/
    replaceIfPresentFlag(0), /**/
    smDefaultMsgId(0), /**/
    nullTerminateOctetStrings(true), /**/
    csmsMethod(SmppClient::CSMS_16BIT_TAGS), /**/
    msgRefCallback(&SmppClient::defaultMessageRef), /**/
    state(OPEN), /**/
    socket(_socket), /**/
    seqNo(0), /**/
    pdu_queue(), /**/
    socketWriteTimeout(5000), /**/
    socketReadTimeout(30000), /**/
    verbose(false) {
}

SmppClient::~SmppClient() {
    try {
        if (state != OPEN) {
            unbind();
        }
    } catch (std::exception &e) {
    }
}

void SmppClient::bindTransmitter(const string &login, const string &pass) {
    bind(smpp::BIND_TRANSMITTER, login, pass);
}

void SmppClient::bindReceiver(const string &login, const string &pass) {
    bind(smpp::BIND_RECEIVER, login, pass);
}

void SmppClient::bind(uint32_t mode, const string &login, const string &password) {
    checkConnection();
    checkState(OPEN);
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

PDU SmppClient::setupBindPdu(uint32_t mode, const string &login, const string &password) {
    PDU pdu(mode, 0, nextSequenceNumber());
    pdu << login;
    pdu << password;
    pdu << systemType;
    pdu << interfaceVersion;
    pdu << addrTon;
    pdu << addrNpi;
    pdu << addrRange;
    return pdu;
}

void SmppClient::unbind() {
    checkConnection();
    PDU pdu(smpp::UNBIND, 0, nextSequenceNumber());
    PDU resp = sendCommand(pdu);
    uint32_t pduStatus = resp.getCommandStatus();

    if (pduStatus != smpp::ESME_ROK) {
        throw smpp::SmppException(smpp::getEsmeStatus(pduStatus));
    }

    state = OPEN;
}

/**
 * Send an sms to the smsc.
 */
pair<string, int> SmppClient::sendSms(const SmppAddress &sender, const SmppAddress &receiver, const string &shortMessage,
                           list<TLV> tags, const uint8_t priority_flag, const string &schedule_delivery_time,
                           const string &validity_period, const int dataCoding) {
    int messageLen = shortMessage.length();
    int singleSmsOctetLimit = 254;  // Default SMPP standard
    int csmsSplit = -1;  // where to split

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
    if (messageLen <= singleSmsOctetLimit || csmsMethod == CSMS_PAYLOAD) {
        string smscId = submitSm(sender, receiver, shortMessage, tags, priority_flag, schedule_delivery_time, validity_period,
                        esmClass, dataCoding);
        return std::make_pair(smscId, 1);
    }

    // CSMS -> split message
    vector<string> parts = split(shortMessage, csmsSplit);
    vector<string>::iterator itr = parts.begin();

    if (csmsMethod == CSMS_8BIT_UDH) {
        // encode an udh with an 8bit csms reference
        uint8_t segment = 0;
        uint8_t segments = numeric_cast<uint8_t>(parts.size());
        string smsId;
        uint8_t csmsRef = static_cast<uint8_t>(msgRefCallback() & 0xff);

        for (; itr < parts.end(); itr++) {
            // encode udh
            int partSize = (*itr).size();
            int size = 6 + partSize;
            boost::scoped_array<uint8_t> udh(new uint8_t[size]);
            udh[0] = 0x05;  // length of udh excluding first byte
            udh[1] = 0x00;  //
            udh[2] = 0x03;  // length of the header
            udh[3] = csmsRef;
            udh[4] = segments;
            udh[5] = ++segment;
            // concatenate with message part
            copy((*itr).begin(), (*itr).end(), &udh[6]);
            string message(reinterpret_cast<char*>(udh.get()), size);
            smsId = submitSm(sender, receiver, message, tags, priority_flag, schedule_delivery_time, validity_period,
                             esmClass | 0x40, dataCoding);
        }

        return std::make_pair(smsId, segments);
    } else {  // csmsMethod == CSMS_16BIT_TAGS)
        tags.push_back(TLV(smpp::tags::SAR_MSG_REF_NUM, static_cast<uint16_t>(msgRefCallback())));
        tags.push_back(TLV(smpp::tags::SAR_TOTAL_SEGMENTS, boost::numeric_cast<uint8_t>(parts.size())));
        int segment = 0;
        string smsId;

        for (; itr < parts.end(); itr++) {
            tags.push_back(TLV(smpp::tags::SAR_SEGMENT_SEQNUM, ++segment));
            smsId = submitSm(sender, receiver, (*itr), tags, priority_flag, schedule_delivery_time, validity_period,
                             esmClass, dataCoding);
            // pop SAR_SEGMENT_SEQNUM tag
            tags.pop_back();
        }

        // pop SAR_TOTAL_SEGMENTS tag
        tags.pop_back();
        // pop SAR_MSG_REF_NUM tag
        tags.pop_back();
        return std::make_pair(smsId, segment);
    }
}

SMS SmppClient::readSms() {
    // see if we're bound correct.
    checkState(BOUND_RX);

    // if  there are any messages in the queue pop the first usable one off and return it
    if (!pdu_queue.empty()) {
        return parseSms();
    }

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

            if (!pdu.null) {
                pdu_queue.push_back(pdu);    // save pdu for reading later
            }

            b = pdu.getCommandId() == DELIVER_SM;
        }
    } catch (std::exception &e) {
        throw smpp::TransportException(e.what());
    }

    return parseSms();
}

QuerySmResult SmppClient::querySm(std::string messageid, SmppAddress source) {
    PDU pdu = PDU(QUERY_SM, 0, nextSequenceNumber());
    pdu << messageid;
    pdu << source.ton;
    pdu << source.npi;
    pdu << source.value;
    PDU reply = sendCommand(pdu);
    string msgid;
    string final_date;
    uint8_t message_state;
    uint8_t error_code;
    reply >> msgid;
    reply >> final_date;
    reply >> message_state;
    reply >> error_code;
    local_date_time ldt(not_a_date_time);

    if (final_date.length() > 1) {
        smpp::timeformat::DatePair p = smpp::timeformat::parseSmppTimestamp(final_date);
        ldt = p.first;
    }

    return QuerySmResult(msgid, ldt, message_state, error_code);
}

void SmppClient::enquireLink() {
    PDU pdu = PDU(ENQUIRE_LINK, 0, nextSequenceNumber());
    sendCommand(pdu);
}

SMS SmppClient::parseSms() {
    if (pdu_queue.empty()) {
        return SMS();
    }

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

vector<string> SmppClient::split(const string &shortMessage, const int split) {
    vector<string> parts;
    int len = shortMessage.length();
    int pos = 0;
    int n = split;

    while (pos < len) {
        if (static_cast<int>(shortMessage[pos + n - 1]) == 0x1b) {  // do not split at escape char
            n--;
        }

        parts.push_back(shortMessage.substr(pos, n));
        pos += n;
        n = split;

        if (pos + n > len) {
            n = len - pos;
        }
    }

    return parts;
}

string SmppClient::submitSm(const SmppAddress &sender, const SmppAddress &receiver, const string &shortMessage,
                            list<TLV> tags, const uint8_t priority_flag, const string &schedule_delivery_time,
                            const string &validity_period, const int esmClassOpt, const int dataCoding) {
    checkState(BOUND_TX);
    PDU pdu(smpp::SUBMIT_SM, 0, nextSequenceNumber());
    pdu << serviceType;
    pdu << sender;
    pdu << receiver;
    pdu << esmClassOpt;
    pdu << protocolId;
    pdu << priority_flag;
    pdu << schedule_delivery_time;
    pdu << validity_period;
    pdu << registeredDelivery;
    pdu << replaceIfPresentFlag;
    pdu << dataCoding;
    pdu << smDefaultMsgId;

    if (csmsMethod == CSMS_PAYLOAD) {
        pdu << 0;  // sm_length = 0
        pdu << TLV(smpp::tags::MESSAGE_PAYLOAD, shortMessage);
    } else {
        pdu.setNullTerminateOctetStrings(nullTerminateOctetStrings);
        pdu << boost::numeric_cast<uint8_t>(shortMessage.length()) + (nullTerminateOctetStrings ? 1 : 0);
        pdu << shortMessage;
        pdu.setNullTerminateOctetStrings(true);
    }

    // add  optional tags.
    for (list<TLV>::iterator itr = tags.begin(); itr != tags.end(); itr++) {
        pdu << *itr;
    }

    PDU resp = sendCommand(pdu);
    string messageid;
    resp >> messageid;
    return messageid;
}

uint32_t SmppClient::nextSequenceNumber() {
    if (++seqNo > 0x7FFFFFFF) {
        throw SmppException("Ran out of sequence numbers");
    }

    return seqNo;
}

void SmppClient::sendPdu(PDU &pdu) {
    checkConnection();
    optional<error_code> ioResult;
    optional<error_code> timerResult;

    if (verbose) {
        LOG(INFO) << pdu;
    }

    deadline_timer timer(getIoService());
    timer.expires_from_now(boost::posix_time::milliseconds(socketWriteTimeout));
    timer.async_wait(boost::bind(&SmppClient::handleTimeout, this, &timerResult, _1));
    async_write(*socket, buffer(pdu.getOctets().get(), pdu.getSize()),
                boost::bind(&SmppClient::writeHandler, this, &ioResult, _1));
    socketExecute();

    if (ioResult) {
        timer.cancel();
    } else if (timerResult) {
        socket->cancel();
    }

    socketExecute();
}

PDU SmppClient::sendCommand(PDU &pdu) {
    sendPdu(pdu);
    PDU resp = readPduResponse(pdu.getSequenceNo(), pdu.getCommandId());

    switch (resp.getCommandStatus()) {
    case smpp::ESME_RINVPASWD:
        throw smpp::InvalidPasswordException(smpp::getEsmeStatus(resp.getCommandStatus()));
        break;

    case smpp::ESME_RINVSYSID:
        throw smpp::InvalidSystemIdException(smpp::getEsmeStatus(resp.getCommandStatus()));
        break;

    case smpp::ESME_RINVSRCADR:
        throw smpp::InvalidSourceAddressException(smpp::getEsmeStatus(resp.getCommandStatus()));
        break;

    case smpp::ESME_RINVDSTADR:
        throw smpp::InvalidDestinationAddressException(smpp::getEsmeStatus(resp.getCommandStatus()));
        break;
    }

    if (resp.getCommandStatus() != smpp::ESME_ROK) {
        throw smpp::SmppException(smpp::getEsmeStatus(resp.getCommandStatus()));
    }

    return resp;
}

PDU SmppClient::readPdu(const bool &isBlocking) {
    // return NULL pdu if there is nothing on the wire for us.
    if (!isBlocking && !socketPeek()) {
        return PDU();
    }

    readPduBlocking();

    // There are no pdus to be read return a null pdu.
    if (pdu_queue.empty()) {
        return PDU();
    }

    // Return last the pdu inserted into the queue.
    PDU pdu = pdu_queue.back();
    pdu_queue.pop_back();

    if (verbose) {
        LOG(INFO) << pdu;
    }

    return pdu;
}

bool SmppClient::socketPeek() {
    // prepare our read
    shared_array<uint8_t> pduHeader(new uint8_t[4]);
    async_read(*socket, buffer(pduHeader.get(), 4),
               boost::bind(&smpp::SmppClient::readPduHeaderHandler, this, _1, _2, pduHeader));
    size_t handlersCalled = getIoService().poll_one();
    getIoService().reset();
    socket->cancel();
    socketExecute();
    return handlersCalled != 0;
}

void SmppClient::readPduBlocking() {
    optional<error_code> ioResult;
    optional<error_code> timerResult;
    shared_array<uint8_t> pduHeader(new uint8_t[4]);
    async_read(*socket, boost::asio::buffer(pduHeader.get(), 4),
               boost::bind(&SmppClient::readPduHeaderHandlerBlocking, this, &ioResult, _1, _2, pduHeader));
    deadline_timer timer(getIoService());
    timer.expires_from_now(boost::posix_time::milliseconds(socketReadTimeout));
    timer.async_wait(boost::bind(&SmppClient::handleTimeout, this, &timerResult, _1));
    socketExecute();

    if (ioResult) {
        timer.cancel();
    } else if (timerResult) {
        socket->cancel();
    }

    socketExecute();
}

void SmppClient::handleTimeout(optional<error_code>* opt, const error_code &error) {
    opt->reset(error);
}

void SmppClient::writeHandler(optional<error_code>* opt, const error_code &error) {
    opt->reset(error);

    if (error) {
        throw TransportException(system_error(error).what());
    }
}

void SmppClient::socketExecute() {
    getIoService().run_one();
    getIoService().reset();
}

void SmppClient::readPduHeaderHandler(const error_code &error, size_t len, const shared_array<uint8_t> &pduLength) {
    if (error) {
        if (error == boost::asio::error::operation_aborted) {
            // Not treated as an error
            return;
        }

        throw TransportException(system_error(error).what());
    }

    uint32_t i = PDU::getPduLength(pduLength);
    shared_array<uint8_t> pduBuffer(new uint8_t[i]);
    // start reading after the size mark of the pdu
    async_read(*socket, buffer(pduBuffer.get(), i - 4),
               boost::bind(&smpp::SmppClient::readPduBodyHandler, this, _1, _2, pduLength, pduBuffer));
    socketExecute();
}

void SmppClient::readPduHeaderHandlerBlocking(optional<error_code>* opt, const error_code &error, size_t read,
        shared_array<uint8_t> pduLength) {
    if (error) {
        if (error == boost::asio::error::operation_aborted) {
            // Not treated as an error
            return;
        }

        throw TransportException(system_error(error).what());
    }

    opt->reset(error);
    uint32_t i = PDU::getPduLength(pduLength);
    shared_array<uint8_t> pduBuffer(new uint8_t[i - 4]);
    // start reading after the size mark of the pdu
    async_read(*socket, buffer(pduBuffer.get(), i - 4),
               boost::bind(&smpp::SmppClient::readPduBodyHandler, this, _1, _2, pduLength, pduBuffer));
    socketExecute();
}

void SmppClient::readPduBodyHandler(const error_code &error, size_t len, shared_array<uint8_t> pduLength,
                                    shared_array<uint8_t> pduBuffer) {
    if (error) {
        throw TransportException(system_error(error).what());
    }

    PDU pdu(pduLength, pduBuffer);
    pdu_queue.push_back(pdu);
}

// blocks until response is read
PDU SmppClient::readPduResponse(const uint32_t &sequence, const uint32_t &commandId) {
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
            if ((pdu.getSequenceNo() == sequence && (pdu.getCommandId() == response
                    || pdu.getCommandId() == GENERIC_NACK))
                    || (pdu.getSequenceNo() == 0 && pdu.getCommandId() == GENERIC_NACK)) {
                return pdu;
            }
        }
    }

    PDU pdu;
    return pdu;
}

void smpp::SmppClient::enquireLinkRespond() {
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

void SmppClient::checkConnection() {
    if (!socket->is_open()) {
        throw smpp::TransportException("Socket is closed");
    }
}

void SmppClient::checkState(int state) {
    if (this->state != state) {
        throw smpp::SmppException("Client in wrong state");
    }
}

uint16_t SmppClient::defaultMessageRef() {
    static int ref = 0;
    return (ref++ % 0xffff);
}

}  // namespace smpp
