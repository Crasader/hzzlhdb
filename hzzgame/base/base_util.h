
#ifndef BASE_UTIL_H_
#define BASE_UTIL_H_

#include <string>

#include "basictypes.h"

class BaseUtil
{
public:
	static std::string MD5Encrypt(const std::string& src);

private:
	DISALLOW_IMPLICIT_CONSTRUCTORS(BaseUtil);
};

#endif // BASE_UTIL_H_