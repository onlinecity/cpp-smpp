#include "pdu.h"
#include "smpp.h"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

const shared_array<uint8_t> smpp::PDU::getOctets()
{
	uint32_t size = getSize();
	buf.seekp(0);
	buf << size;

	buf.seekg(0);

	shared_array<uint8_t> octets(new uint8_t[size]);
	buf.read((char*) octets.get(), size);
	return octets;
}

const int smpp::PDU::getSize()
{
	buf.seekp(0, ios_base::end);
	return buf.tellp();
}

const uint32_t smpp::PDU::getCommandId()
{
	return cmdId;
}

const uint32_t smpp::PDU::getCommandStatus()
{
	return cmdStatus;
}

const uint32_t smpp::PDU::getSequenceNo()
{
	return seqNo;
}

const bool smpp::PDU::isNullTerminating()
{
	return nullTerminateOctetStrings;
}

void smpp::PDU::setNullTerminateOctetStrings(const bool &b)
{
	nullTerminateOctetStrings = b;
}

inline smpp::PDU& smpp::PDU::operator+=(const int &i)
{
	buf << (uint8_t) (i & 0xff);
	return *this;
}

inline smpp::PDU& smpp::PDU::operator+=(const uint8_t &i)
{
	buf << (i & 0xff);
	return *this;
}

inline smpp::PDU& smpp::PDU::operator+=(const uint16_t &i)
{
	buf << (i & 0xffff);
	return *this;
}

inline smpp::PDU& smpp::PDU::operator+=(const uint32_t &i)
{
	buf << (i & 0xffffffff);
	return *this;
}

smpp::PDU& smpp::PDU::operator+=(std::basic_string<char> s)
{
	buf << nullTerminateOctetStrings ? s.c_str() : s.data();

	return *this;
}

smpp::PDU& smpp::PDU::operator +=(const smpp::SmppAddress s)
{
	(*this) += s.ton;
	(*this) += s.npi;
	(*this) += s.value;
	return *this;
}

smpp::PDU& smpp::PDU::operator +=(smpp::TLV tlv)
{
	(*this) += tlv.getTag();
	(*this) += tlv.getLen();
	if (tlv.getLen() != 0) {

		(*this).addOctets(tlv.getOctets(), (uint32_t) tlv.getLen());
	}
	return *this;
}

smpp::PDU& smpp::PDU::addOctets(const shared_array<uint8_t> &octets, const streamsize &len)
{
	buf.write((char*) octets.get(), len);
	return *this;
}

void smpp::PDU::skip(int octets)
{
	buf.seekg(octets, ios_base::cur);
}

void smpp::PDU::resetMarker()
{
	buf.seekg(0);
}

uint16_t smpp::PDU::read2Int()
{
	uint16_t i;
	buf >> i;
	return i;
}

uint32_t smpp::PDU::read4Int()
{
	uint32_t i;
	buf >> i;
	return i;
}

int smpp::PDU::readInt()
{
	uint64_t i;
	buf >> i;
	return i;
}

string smpp::PDU::readString()
{

	string s;
	buf >> s;
	return s;
}

void smpp::PDU::readOctets(shared_array<uint8_t> &octets, const streamsize &len)
{
	buf.readsome((char*) octets.get(), len);
}

bool smpp::PDU::hasMoreData()
{
	return buf.eof();
}

std::ostream& smpp::operator<<(std::ostream& out, smpp::PDU& pdu)
{
	using namespace std;

	if (pdu.null) {
		out << "PDU IS NULL" << endl;
		return out;
	}

	out << "words     :" << pdu.getSize() << endl;
	out << "sequence  :" << pdu.seqNo << endl;
	out << "cmd id    :0x" << hex << pdu.cmdId << dec << endl;
	out << "cmd status:0x" << hex << pdu.cmdStatus << dec << " : " << smpp::getEsmeStatus(pdu.cmdStatus) << endl;

	out << setw(2) << setfill('0') << hex << pdu.buf;

	out << dec << endl << endl;
	return out;
}
