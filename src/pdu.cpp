#include "pdu.h"
#include "smpp.h"
#include <cstring>

bool smpp::PDU::checkBounds(int i) {
	return size + i < capacity;
}

void smpp::PDU::pack() {
	packHeaderField(size, 0);
	packHeaderField(cmdId, HEADERFIELD_SIZE);
	packHeaderField(cmdStatus, HEADERFIELD_SIZE * 2);
	packHeaderField(seqNo, HEADERFIELD_SIZE * 3);
}

void smpp::PDU::packHeaderField(uint32_t i, int offset) {
	buffer[offset + 0] = (i >> 24) & 0xff;
	buffer[offset + 1] = (i >> 16) & 0xff;
	buffer[offset + 2] = (i >> 8) & 0xff;
	buffer[offset + 3] = i & 0xff;
}

void smpp::PDU::extractHeaderField(uint32_t &i, int offset) {
	int shift = 8;
	for (int j = 0; j < 4; j++) {
		i <<= shift;
		i |= (uint32_t) buffer[j + offset];
	}
}

uint8_t* smpp::PDU::getOctets() {
	pack();
	return buffer;
}

const int smpp::PDU::getSize() {
	return size;
}

const uint32_t smpp::PDU::getCommandId() {
	return cmdId;
}

const uint32_t smpp::PDU::getCommandStatus() {
	return cmdStatus;
}

const uint32_t smpp::PDU::getSequenceNo() {
	return seqNo;
}

const bool smpp::PDU::isNullTerminating() {
	return nullTerminateOctetStrings;
}

void smpp::PDU::setNullTerminateOctetStrings(const bool &b) {
	nullTerminateOctetStrings = b;
}

smpp::PDU& smpp::PDU::operator+=(const int &i) {
	if (!checkBounds(1))
		return *this;

	buffer[size++] = i & 0xff;
	return *this;
}

smpp::PDU& smpp::PDU::operator+=(const uint8_t &i) {
	if (!checkBounds(1))
		return *this;

	buffer[size++] = i & 0xff;
	return *this;
}

smpp::PDU& smpp::PDU::operator+=(const uint16_t &i) {
	if (!checkBounds(2))
		return *this;
	buffer[size++] = (i >> 8) & 0xff;
	buffer[size++] = i & 0xff;
	return *this;
}

smpp::PDU& smpp::PDU::operator+=(const uint32_t &i) {
	if (!checkBounds(4))
		return *this;

	buffer[size++] = (i >> 24) & 0xff;
	buffer[size++] = (i >> 16) & 0xff;
	buffer[size++] = (i >> 8) & 0xff;
	buffer[size++] = i & 0xff;
	return *this;
}

smpp::PDU& smpp::PDU::operator+=(std::basic_string<char> s) {
	if (!checkBounds(s.length()))
		return *this;

	std::copy(s.begin(), s.end(), buffer + size);
	size += s.length() + (nullTerminateOctetStrings ? 1 : 0);
	if (nullTerminateOctetStrings)
		buffer[size - 1] = 0;
	return *this;
}

smpp::PDU& smpp::PDU::operator +=(const smpp::SmppAddress s) {
	(*this) += s.ton;
	(*this) += s.npi;
	(*this) += s.value;
	return *this;
}

smpp::PDU& smpp::PDU::operator +=(smpp::TLV tlv) {
	(*this) += tlv.getTag();
	(*this) += tlv.getLen();
	if (tlv.getLen() != 0) {
		(*this).addOctets(tlv.getOctets(), (uint32_t) tlv.getLen());
	}
	return *this;
}

smpp::PDU& smpp::PDU::addOctets(const uint8_t *arr, const uint32_t &l) {
	std::copy(arr, arr + l, buffer + size);
	size += l;
	return *this;
}

void smpp::PDU::skip(int octets) {
	marker += octets;
	if (marker >= size)
		resetMarker();
}

void smpp::PDU::resetMarker() {
	marker = HEADER_SIZE;
}

uint16_t smpp::PDU::read2Int() {
	int shift = 8;
	int i = 0;
	i <<= shift;
	i |= (uint16_t) buffer[marker];

	i <<= shift;
	i |= (uint16_t) buffer[marker + 1];

	marker += 2;

	return i;
}

uint32_t smpp::PDU::read4Int() {
	uint32_t i = 0;
	int shift = 8;
	for (int j = 0; j < 4; j++) {
		i <<= shift;
		i |= (uint32_t) buffer[j + marker];
	}
	marker += 4;
	return i;
}

int smpp::PDU::readInt() {
	int i = buffer[marker];
	marker++;
	return i;
}

uint8_t* smpp::PDU::readStr() {
	size_t len = strlen((char*) &buffer[marker]) + 1;
	uint8_t *s = new uint8_t[len];
	std::copy(buffer + marker, buffer + marker + len, s);
	s[len] = 0x0;
	marker += len;
	return s;
}

string smpp::PDU::readString() {
	stringstream ss;
	string s;
	ss << readStr();
	ss >> s;
	return s;
}

void smpp::PDU::readStr(uint8_t *arr, const uint32_t &len) {
	std::copy(buffer + marker, buffer + (marker + len), arr);
	marker += len;
	arr[len] = 0;
}

void smpp::PDU::readOctets(uint8_t *arr, const uint32_t &len) {
	std::copy(buffer + marker, buffer + (marker + len), arr);
	marker += len;
}

bool smpp::PDU::hasMoreData() {
	return marker < size;
}

std::ostream& smpp::operator<<(std::ostream& out, smpp::PDU& pdu) {
	using namespace std;

	if (pdu.null) {
		out << "PDU IS NULL" << endl;
		return out;
	}

	pdu.pack();

	out << "words     :" << pdu.size << endl;
	out << "sequence  :" << pdu.seqNo << endl;
	out << "cmd id    :0x" << hex << pdu.cmdId << dec << endl;
	out << "cmd status:0x" << hex << pdu.cmdStatus << dec << " : "
			<< smpp::getEsmeStatus(pdu.cmdStatus) << endl;

	for (int i = 0; i < pdu.size; i++) {
		if (pdu.marker == i) {
			out << ">" << setw(2) << setfill('0') << hex << (int) pdu.buffer[i]
					<< "< ";
			continue;
		}
		out << setw(2) << setfill('0') << hex << (int) pdu.buffer[i] << " ";
	}

	out << dec << endl << endl;
	return out;
}
