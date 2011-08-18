#ifndef PDU_H_
#define PDU_H_
#include "smpp.h"
#include "tlv.h"
#include <stdint.h> // types
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

namespace smpp {

const int HEADERFIELD_SIZE = 4;
const int HEADER_SIZE = HEADERFIELD_SIZE * 4;

/**
 * Class for representing a PDU.
 */
class PDU {
private:
	int capacity;
	int size;
public:
	int marker;
private:
	uint32_t cmdId;
	uint32_t cmdStatus;
	uint32_t seqNo;
	uint8_t *buffer;

	bool nullTerminateOctetStrings;

	void pack();
	void packHeaderField(uint32_t val, int pos);
	void extractHeaderField(uint32_t &val, int pos);
	bool checkBounds(int i);
public:
	bool null;

public:

	PDU() :
			capacity(0), size(0), marker(0), cmdId(0), cmdStatus(0), seqNo(0), buffer(
					new uint8_t[size]), nullTerminateOctetStrings(true), null(true) {
	}

	PDU(uint32_t _cmdId, uint32_t _cmdStatus, uint32_t _seqNo, int _initCapacity =
			512) :
			capacity(_initCapacity), size(HEADER_SIZE), marker(HEADER_SIZE), cmdId(
					_cmdId), cmdStatus(_cmdStatus), seqNo(_seqNo), buffer(
					new uint8_t[HEADER_SIZE + capacity]), nullTerminateOctetStrings(true), null(false)
					{
						packHeaderField(cmdId, HEADERFIELD_SIZE);
						packHeaderField(cmdStatus, HEADERFIELD_SIZE * 2);
						packHeaderField(seqNo, HEADERFIELD_SIZE * 3);
					}

					PDU(uint8_t* _buffer, int _size) : capacity(_size),
					size(_size), marker(HEADER_SIZE), buffer(_buffer), null(false)
					{
						extractHeaderField(cmdId, HEADERFIELD_SIZE);
						extractHeaderField(cmdStatus, HEADERFIELD_SIZE * 2);
						extractHeaderField(seqNo, HEADERFIELD_SIZE * 3);
					}

					PDU(const PDU &rhs) :
					capacity (rhs.capacity),
					size(rhs.size),
					marker (rhs.marker),

					cmdId(rhs.cmdId),
					cmdStatus(rhs.cmdStatus),
					seqNo (rhs.seqNo),
					nullTerminateOctetStrings(rhs.nullTerminateOctetStrings),

					null(rhs.null) {

						if (!null) {

							buffer = new uint8_t[capacity];
							std::copy(rhs.buffer, rhs.buffer + capacity, buffer);
						}
					}

					const PDU& operator= (const PDU &rhs) {
						if (this != &rhs) {
							capacity = rhs.capacity;
							size = rhs.size;
							marker = rhs.marker;
							cmdId = rhs.cmdId;
							cmdStatus = rhs.cmdStatus;
							seqNo = rhs.seqNo;
							nullTerminateOctetStrings = rhs.nullTerminateOctetStrings;
							null = rhs.null;
							if (!null) {

								buffer = new uint8_t[capacity];
								std::copy(rhs.buffer, rhs.buffer + capacity, buffer);
							}
						}

						return *this;
					}

					~PDU() {
						delete[] buffer;
					}

					/**
					 * @return All data in this PDU as array of unsigned char array.
					 */
					uint8_t* getOctets();

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

					PDU& operator+=(const int &);
					PDU& operator+=(const uint8_t &i);
					PDU& operator+=(const uint16_t &i);
					PDU& operator+=(const uint32_t &i);
					PDU& operator+=(std::basic_string<char> s);
					PDU& operator+=(const smpp::SmppAddress);
					PDU& operator+=(const smpp::TLV);
					PDU& addOctets(const uint8_t*, const uint32_t &);

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
					 * @return Next octets until a 0x00 is read as a c-style string.
					 */
					uint8_t* readStr();

					/**
					 * Copy n octets into a string. Adds an 0x00 at the end.
					 * @param s target string
					 * @param n Octets to copy
					 */
					void readStr(uint8_t *s, const uint32_t &n );

					/**
					 * Copy n octet into an array.
					 * @param array Target array.
					 * @param n Octets to copy.
					 */
					void readOctets(uint8_t *s, const uint32_t &n);

					/**
					 * @return True if the read marker is not at the end of the PDU.
					 */
					bool hasMoreData();

					friend std::ostream& smpp::operator<<(std::ostream&, smpp::PDU&);
				};

std::ostream& operator<<(std::ostream&, smpp::PDU&);
}

#endif /* PDU_H_ */
