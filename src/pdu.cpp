/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#include "pdu.h"

using namespace std;
using namespace boost;

namespace smpp {

const shared_array<uint8_t> PDU::getOctets()
{
	uint32_t size = getSize();
	uint32_t beSize = htonl(size);

	buf.seekp(0, ios::beg);

	buf.write(reinterpret_cast<char*>(&beSize), sizeof(uint32_t));
	buf.seekp(0, ios::end);
	buf.seekg(0, ios::beg);

	shared_array<uint8_t> octets(new uint8_t[size]);
	buf.read(reinterpret_cast<char*>(octets.get()), size);

	// Seek to start of PDU body (after headers)
	buf.seekg(HEADERFIELD_SIZE * 4, ios::beg);

	return octets;
}

int PDU::getSize()
{
	buf.seekp(0, ios_base::end);
	int s = buf.tellp();

	return s;
}

uint32_t PDU::getCommandId() const
{
	return cmdId;
}

uint32_t PDU::getCommandStatus() const
{
	return cmdStatus;
}

uint32_t PDU::getSequenceNo() const
{
	return seqNo;
}

bool PDU::isNullTerminating() const
{
	return nullTerminateOctetStrings;
}

void PDU::setNullTerminateOctetStrings(const bool &b)
{
	nullTerminateOctetStrings = b;
}

PDU& PDU::operator<<(const int &i)
{
	uint8_t x(i);
	buf.write(reinterpret_cast<char*>(&x), sizeof(uint8_t));
	return *this;
}

PDU& PDU::operator<<(const uint8_t &i)
{
	uint8_t x(i);
	buf.write(reinterpret_cast<char*>(&x), sizeof(uint8_t));
	return *this;
}

PDU& PDU::operator<<(const uint16_t &i)
{
	uint16_t j = htons(i);
	buf.write(reinterpret_cast<char*>(&j), sizeof(uint16_t));
	return *this;
}

PDU& PDU::operator<<(const uint32_t &i)
{
	uint32_t j = htonl(i);
	buf.write(reinterpret_cast<char*>(&j), sizeof(uint32_t));
	return *this;
}

PDU& PDU::operator<<(const std::basic_string<char> &s)
{
	buf.write(s.c_str(), s.length()); // use buf.write to allow for UCS-2 chars which are 16-bit.
	if (nullTerminateOctetStrings) {
		buf << ends;
	}

	return *this;
}

PDU& PDU::operator <<(const smpp::SmppAddress s)
{
	(*this) << s.ton;
	(*this) << s.npi;
	(*this) << s.value;
	return *this;
}

PDU& PDU::operator <<(smpp::TLV tlv)
{
	(*this) << tlv.getTag();
	(*this) << tlv.getLen();
	if (tlv.getLen() != 0) {

		(*this).addOctets(tlv.getOctets(), (uint32_t) tlv.getLen());
	}
	return *this;
}

PDU& PDU::addOctets(const shared_array<uint8_t> &octets, const streamsize &len)
{

	buf.write(reinterpret_cast<char*>(octets.get()), len);
	return *this;
}

void PDU::skip(int octets)
{
	buf.seekg(octets, ios_base::cur);
}

void PDU::resetMarker()
{
	// Seek to start of PDU body (after headers)
	buf.seekg(HEADERFIELD_SIZE * 4, ios::beg);
}

PDU& PDU::operator>>(int &i)
{
	uint8_t j;
	buf.read(reinterpret_cast<char*>(&j), sizeof(uint8_t));
	i = j;
	return *this;
}

PDU& PDU::operator>>(uint8_t &i)
{
	buf.read(reinterpret_cast<char*>(&i), sizeof(uint8_t));
	return *this;
}

PDU& PDU::operator>>(uint16_t &i)
{
	buf.read(reinterpret_cast<char*>(&i), sizeof(uint16_t));
	i = ntohs(i);
	return *this;
}

PDU& PDU::operator>>(uint32_t &i)
{
	buf.read(reinterpret_cast<char*>(&i), sizeof(uint32_t));
	i = ntohl(i);
	return *this;
}

PDU& PDU::operator>>(std::basic_string<char> &s)
{
	getline(buf, s, '\0');
	return *this;
}

void PDU::readOctets(shared_array<uint8_t> &octets, const streamsize &len)
{
	buf.readsome(reinterpret_cast<char*>(octets.get()), len);
}

bool PDU::hasMoreData()
{
	return buf.eof();
}
}

std::ostream &smpp::operator<<(std::ostream& out, smpp::PDU& pdu)
{
	if (pdu.null) {
		out << "PDU IS NULL" << std::endl;
		return out;
	}

	int size = pdu.getSize();

	out << "size      :" << size << std::endl;
	out << "sequence  :" << pdu.seqNo << std::endl;
	out << "cmd id    :0x" << hex << pdu.cmdId << dec << std::endl;
	out << "cmd status:0x" << hex << pdu.cmdStatus << dec << " : " << smpp::getEsmeStatus(pdu.cmdStatus) << std::endl;

	shared_array<uint8_t> octets = pdu.getOctets();
	for (int i = 0 ; i < size ; i++) {
		out << std::setw(2) << std::setfill('0') << std::hex << boost::numeric_cast<int>(octets[i]) << " ";
	}

	out << dec << std::endl;
	return out;
}

namespace smpp {
uint32_t getPduLength(boost::shared_array<uint8_t> pduHeader)
{
	uint32_t* i = reinterpret_cast<uint32_t*>(pduHeader.get());

return	ntohl(*i);
}}
