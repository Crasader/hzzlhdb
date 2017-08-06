
#ifndef QPCIPHER_H_
#define QPCIPHER_H_

#include "../base/basictypes.h"
#include "../../../../global/define.h"
#include "../../../../global/Packet.h"

// º”√‹Ω‚√‹¿‡
class QPCipher
{
public:
	static uint8* EncryptBuffer(uint8* data, int dataSize);
	static uint8* DecryptBuffer(uint8* data, int start, int dataSize);

	static int GetCipherMode();

	static int GetMainCommand(uint8* data, int start);
	static int GetSubConmmand(uint8* data, int start);
	static int GetPackSize(uint8* data, int start);
	static void SetPackInfo(uint8* data, int dataSize, int main, int sub);

	static int GetCipher(uint8* data, int start);
	static int GetCipherCode(uint8* data, int start);

private:
	DISALLOW_IMPLICIT_CONSTRUCTORS(QPCipher);
};

#endif	// QPCIPHER_H_
