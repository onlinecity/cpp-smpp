#ifndef PDU_H_
#define PDU_H_
#include "smpp.h"
#include "tlv.h"
#include <stdint.h> // types
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <boost/shared_array.hpp>
#include <netinet/in.h>

using namespace std;
using namespace boost;

namespace smpp {

const int HEADERFIELD_SIZE = 4;
const int HEADER_SIZE = HEADERFIELD_SIZE * 4;

/**
 * Class for representing a PDU.
 */
class PDU
{
private:
	stringbuf sb;
	iostream buf;
	uint32_t cmdId;
	uint32_t cmdStatus;
	uint32_t seqNo;
	bool nullTerminateOctetStrings;
public:
	bool null;

public:

	PDU() :
			buf(&sb), cmdId(0), cmdStatus(0), seqNo(0), nullTerminateOctetStrings(true), null(true)
	{
	}

	PDU(uint32_t _cmdId, uint32_t _cmdStatus, uint32_t _seqNo) :
			buf(&sb), cmdId(_cmdId), cmdStatus(_cmdStatus), seqNo(_seqNo), null(false)
	{

//		buf.seekp(HEADERFIELD_SIZE, ios::cur);

		uint32_t size = 0;
		buf.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));

		uint32_t beCmdId = htonl(cmdId);
		buf.write(reinterpret_cast<char*>(&beCmdId), sizeof(uint32_t));

		uint32_t beCmdStatus = htonl(cmdStatus);
		buf.write(reinterpret_cast<char*>(&beCmdStatus), sizeof(uint32_t));

		uint32_t beSeqNo = htonl(seqNo);
		buf.write(reinterpret_cast<char*>(&beSeqNo), sizeof(uint32_t));

	}

	PDU(const shared_array<uint8_t> &pduLength, const shared_array<uint8_t> &pduBuffer) :
			buf(&sb)
	{
		unsigned int bufSize = (int) pduLength[0] << 24 | (int) pduLength[1] << 16 | (int) pduLength[2] << 8
				| (int) pduLength[3];

		sb.sputn((char*) pduLength.get(), HEADERFIELD_SIZE);
		sb.sputn((char*) pduBuffer.get(), bufSize);

		buf.seekg(HEADERFIELD_SIZE, ios::cur);

		buf >> cmdId;
		buf >> cmdStatus;
		buf >> seqNo;

		cmdId = ntohl(cmdId);
		cmdStatus = ntohl(cmdStatus);
		seqNo = ntohl(seqNo);

	}

	/**
	 * @return All data in this PDU as array of unsigned char array.
	 */
	const shared_array<uint8_t> getOctets();

	/**
	 * @return PDU size in octets.
	 */
	const int getSize();

	/**
	 * @return PDU command id.
	 */
	const uint32_t getCommandId();

	/**
	 * @return PDU command status.
	 */
	const uint32_t getCommandStatus();

	/**
	 * @return PDU sequence number.
	 */
	const uint32_t getSequenceNo();

	/**
	 * @return True if null termination is on.
	 */
	const bool isNullTerminating();

	/**
	 * Turns null termination on or off.
	 * @param True if null termination is on.
	 */
	void setNullTerminateOctetStrings(const bool&);

	/** Adds an integer as an unsigned 8 bit. */
	PDU& operator+=(const int &);
	PDU& operator+=(const uint8_t &i);
	PDU& operator+=(const uint16_t &i);
	PDU& operator+=(const uint32_t &i);
	PDU& operator+=(std::basic_string<char> s);
	PDU& operator+=(const smpp::SmppAddress);
	PDU& operator+=(const smpp::TLV);
	PDU& addOctets(const shared_array<uint8_t> &octets, const streamsize &len);

	/**
	 * Skips n octets.
	 * @param n Octets to skip.
	 */
	void skip(int n);

	/**
	 * Resets the read marker to the beginning of the PDU.
	 */
	void resetMarker();

	/**
	 * Returns the next octet.
	 * @return
	 */
	int readInt();

	/**
	 * @return Next two octets as an uint16_t.
	 */
	uint16_t read2Int();
	/**
	 * @return Next four octets as an uint32_t.
	 */
	uint32_t read4Int();

	/**
	 * @return Next null terminated string.
	 */
	string readString();

	/**
	 * Copy n octet into an array.
	 * @param array Target array.
	 * @param n Octets to copy.
	 */
	void readOctets(shared_array<uint8_t> &octets, const streamsize &n);

	/**
	 * @return True if the read marker is not at the end of the PDU.
	 */
	bool hasMoreData();

	friend std::ostream& smpp::operator<<(std::ostream&, smpp::PDU&);
};

std::ostream& operator<<(std::ostream&, smpp::PDU&);
}

#endif /* PDU_H_ */
