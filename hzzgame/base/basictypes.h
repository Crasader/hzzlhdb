
#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#include <assert.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

//------------------------------------------------------------------------------

typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
typedef long long			int64;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

const uint8  kuint8max  = (( uint8) 0xFF);
const uint16 kuint16max = ((uint16) 0xFFFF);
const uint32 kuint32max = ((uint32) 0xFFFFFFFF);
const  int8  kint8min   = ((  int8) 0x80);
const  int8  kint8max   = ((  int8) 0x7F);
const  int16 kint16min  = (( int16) 0x8000);
const  int16 kint16max  = (( int16) 0x7FFF);
const  int32 kint32min  = (( int32) 0x80000000);
const  int32 kint32max  = (( int32) 0x7FFFFFFF);
const  int16 kWordMax = ((int16)0xFFFF);
const  uint32 kDwordMax = ((uint32)0xFFFFFFFF);


#ifndef WIN32
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned int		DWORD;
typedef unsigned short      TCHAR;
typedef int                LONG;
typedef	long long			LONGLONG;
typedef unsigned int       COLORREF;
typedef void				VOID;

struct SYSTEMTIME
{
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
};

#else

#include <winsock2.h>
#include <Windows.h>

#endif

#ifdef _WIN32
#define LLSTRING "%I64d"
#define ULLSTRING "%U64d"
#else
#define LLSTRING "%lld"
#define ULLSTRING "%llu"
#endif

#define makeword(a, b)      ((uint16)(((uint8)(((long long)(a)) & 0xff)) | ((uint16)((uint8)(((long long)(b)) & 0xff))) << 8))
#define makelong(a, b)      ((uint32)(((uint16)(((long long)(a)) & 0xffff)) | ((uint32)((uint16)(((long long)(b)) & 0xffff))) << 16))
#define loword(l)           ((uint16)(((long long)(l)) & 0xffff))
#define hiword(l)           ((uint16)((((long long)(l)) >> 16) & 0xffff))
#define lobyte(w)           ((uint8)(((long long)(w)) & 0xff))
#define hibyte(w)           ((uint8)((((long long)(w)) >> 8) & 0xff))

//------------------------------------------------------------------------------

// ArraySizeHelper是一个返回类型为char[N]的函数,其形参类型为 T[N].
// 函数没必要实现, 因为sizeof只需要类型.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)返回array数组的元素个数. 该表达式是编译时常量,
// 可以用于定义新的数组. 如果传递一个指针会报编译时错误.
#define arraysize(array) (sizeof(ArraySizeHelper(array)))


// DISALLOW_COPY_AND_ASSIGN禁用拷贝和赋值构造函数.
// 需要在类的private:访问控制域中使用.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)

// DISALLOW_IMPLICIT_CONSTRUCTORS禁止隐式的构造函数, 包括缺省构造函数、
// 拷贝构造函数和赋值构造函数.
//
// 需要在类的private:访问控制域中使用以防止实例化, 对于只有静态方法的
// 类非常有用.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
	TypeName();                                    \
	DISALLOW_COPY_AND_ASSIGN(TypeName)


#endif // BASIC_TYPES_H_