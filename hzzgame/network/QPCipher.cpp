
#include "QPCipher.h"

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"

static uint8 MapSendByte(uint8 data)
{
	int len = data;
	if (len >= 256)
	{
		len = len % 256;
	}

	uint8 map = g_SendByteMap[len];
	return map;
}

static uint8 MapRecvByte(uint8 data)
{
	uint8 map = g_RecvByteMap[data];
	return map;
}

uint8* QPCipher::EncryptBuffer(uint8* data, int dataSize)
{
	uint8 check_code = 0;
	for (int i = sizeof(TCP_Info); i < dataSize; i++)
	{
		check_code += data[i];
		data[i] = MapSendByte(data[i]);
	}

	data[0] |= DK_MAPPED;
	data[1] = ~check_code + 1;
    
	return data;
}

uint8* QPCipher::DecryptBuffer(uint8* data, int start, int dataSize)
{
	int check_code = data[start + 1];

	for (int i = start + sizeof(TCP_Info); i < start + dataSize; i++)
	{
		data[i] = MapRecvByte(data[i]);
		check_code += data[i];
	}
    
	return data;
}


int QPCipher::GetCipherMode()
{
	return DK_MAPPED;
}

int QPCipher::GetMainCommand(uint8* data, int start)
{
	int high = data[start + 5];
	int low	 = data[start + 4];
	return (((high & 0xff) << 8) | (low & 0xff));
}

int QPCipher::GetSubConmmand(uint8* data, int start)
{
	int high = data[start + 7];
	int low  = data[start + 6];
	return (((high & 0xff) << 8) | (low & 0xff));
}

int QPCipher::GetPackSize(uint8* data, int start)
{
	int high = data[start + 3];
	int low  = data[start + 2];
	return (((high & 0xff) << 8) | (low & 0xff));
}

void QPCipher::SetPackInfo(uint8* data, int dataSize, int main, int sub)
{
	// 设置包大小
	data[2] = (uint8) (dataSize & 0xff);
	data[3] = (uint8) ((dataSize >> 8) & 0xff);
	// 设置主消息id
	data[4] = (uint8) (main & 0xff);
	data[5] = (uint8) ((main >> 8) & 0xff);
	// 设置子消息id
	data[6] = (uint8) (sub & 0xff);
	data[7] = (uint8) ((sub >> 8) & 0xff);
}


int QPCipher::GetCipher(uint8* data, int start)
{
	int cipher = data[start];	
	return cipher;
}

int QPCipher::GetCipherCode(uint8* data, int start)
{
	int ciphercode = data[start + 1];
	return ciphercode;
}
