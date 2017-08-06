
#ifndef PACKET_HELPER_H_
#define PACKET_HELPER_H_

#include <string>

#include "../base/convert.h"

/**
 * 数据包读取辅助
 */
class PacketHelper
{
private:
	unsigned char* mData;
	int mPos;
public:
	/**
	 * @param data 网络数据
	 * @param index 起始下标
	 */
	PacketHelper(unsigned char* data, int pos = 0);
	void setPosition(int pos);
	int getPosition() const;
	unsigned char* getBuffer();
	unsigned int readByte();
	unsigned int read2Byte();
	unsigned int read4Byte();
	unsigned int* read4Bytes(unsigned int* ary, int count);
	long long read8Byte();

	void writeByte(unsigned int val);
	void write2Byte(unsigned int val);
	void write4Byte(unsigned int val);
	void write4Bytes(unsigned int* ary, int count);
	void write8Byte(long long val);

	/**
	 * @param len (utf16的长度/2)
	 */
	void writeString(const std::string& utf8, int len);
	/**
	 * @param len (utf16的长度/2)
	 */
	std::string readString(int len);
};

#define PACKET_HELPER_SIZE(size) unsigned char __PacketBuffer__[size]={0}; PacketHelper packet(__PacketBuffer__)
#define PACKET_HELPER_DATA(data) PacketHelper packet((unsigned char*)data)

#endif // PACKET_HELPER_H_
