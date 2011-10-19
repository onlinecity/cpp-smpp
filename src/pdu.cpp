#include "pdu.h"
using namespace std;

const shared_array<uint8_t> smpp::PDU::getOctets()
{
	uint32_t size = getSize();
	uint32_t beSize = htonl(size);

	buf.seekp(0, ios::beg);

	buf.write(reinterpret_cast<char*>(&beSize), sizeof(uint32_t));
	buf.seekp(0, ios::end);
	buf.seekg(0, ios::beg);

	shared_array<uint8_t> octets(new uint8_t[size]);
	buf.read((char*) octets.get(), size);
	buf.seekg(0, ios::beg);

	return octets;
}

const int smpp::PDU::getSize()
{
	buf.seekp(0, ios_base::end);
	int s = buf.tellp();

	return s;
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

smpp::PDU& smpp::PDU::operator<<(const int &i)
{
	uint8_t x(i);
	buf.write(reinterpret_cast<char*>(&x), sizeof(uint8_t));
	return *this;
}

smpp::PDU& smpp::PDU::operator<<(const uint8_t &i)
{
	uint8_t x(i);
	buf.write(reinterpret_cast<char*>(&x), sizeof(uint8_t));
	return *this;
}

smpp::PDU& smpp::PDU::operator<<(const uint16_t &i)
{
	uint16_t j = htons(i);
	buf.write(reinterpret_cast<char*>(&j), sizeof(uint16_t));
	return *this;
}

smpp::PDU& smpp::PDU::operator<<(const uint32_t &i)
{
	uint32_t j = htonl(i);
	buf.write(reinterpret_cast<char*>(&j), sizeof(uint32_t));
	return *this;
}

smpp::PDU& smpp::PDU::operator<<(const std::basic_string<char> &s)
{
	buf << s.c_str();
	if (nullTerminateOctetStrings) {
		buf << ends;
	}

	return *this;
}

smpp::PDU& smpp::PDU::operator <<(const smpp::SmppAddress s)
{
	(*this) << s.ton;
	(*this) << s.npi;
	(*this) << s.value;
	return *this;
}

smpp::PDU& smpp::PDU::operator <<(smpp::TLV tlv)
{
	(*this) << tlv.getTag();
	(*this) << tlv.getLen();
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
	buf.seekg(0, ios::beg);
}

smpp::PDU& smpp::PDU::operator>>(int &i)
{
	uint8_t j;
	buf.read(reinterpret_cast<char*>(&j), sizeof(uint8_t));
	i = j;
	return *this;
}

smpp::PDU& smpp::PDU::operator>>(uint8_t &i)
{
	buf.read(reinterpret_cast<char*>(&i), sizeof(uint8_t));
	return *this;
}

smpp::PDU& smpp::PDU::operator>>(uint16_t &i)
{
	buf.read(reinterpret_cast<char*>(&i), sizeof(uint16_t));
	i = ntohs(i);
	return *this;
}

smpp::PDU& smpp::PDU::operator>>(uint32_t &i)
{
	buf.read(reinterpret_cast<char*>(&i), sizeof(uint32_t));
	i = ntohl(i);
	return *this;
}

smpp::PDU& smpp::PDU::operator>>(std::basic_string<char> &s)
{
	getline(buf, s, '\0');
	return *this;
}

void smpp::PDU::readOctets(shared_array<uint8_t> &octets, const streamsize &len)
{
	buf.readsome((char*) octets.get(), len);
}

bool smpp::PDU::hasMoreData()
{
	return buf.eof();
}

std::ostream &smpp::operator<<(std::ostream& out, smpp::PDU& pdu)
{
	using namespace std;

	if (pdu.null) {
		out << "PDU IS NULL" << endl;
		return out;
	}

	int size = pdu.getSize();

	out << "size      :" << size << endl;
	out << "sequence  :" << pdu.seqNo << endl;
	out << "cmd id    :0x" << hex << pdu.cmdId << dec << endl;
	out << "cmd status:0x" << hex << pdu.cmdStatus << dec << " : " << smpp::getEsmeStatus(pdu.cmdStatus) << endl;

	shared_array<uint8_t> octets = pdu.getOctets();

	for (int i = 0 ; i < size ; i++) {
		out << setw(2) << setfill('0') << hex << (int) octets[i] << " ";
	}

	out << dec << endl;
	return out;
}
