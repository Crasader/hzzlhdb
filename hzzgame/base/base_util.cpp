
#include "base_util.h"

#include "MD5.h"

std::string BaseUtil::MD5Encrypt(const std::string& src)
{
	MD5 md5 = MD5(src);
	return md5.hexdigest();
}
