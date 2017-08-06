
#ifndef _Convert2_H_
#define _Convert2_H_
#include <string>

/**
 * 计算utf8字符串长度
 * @param utf8 utf8字符串 
 * @return 字符串长度
 */
int u8len(const std::string& str);

int u8cmp(const std::string& str1, const std::string& str2);

/**
 * 计算ucs2字符串长度
 * @param ucs2 ucs2字符串 
 * @return 字符串长度
 */
int u2len(const std::string& s);

/*UTF8转为gbk*/
std::string u2a(const std::string& s);
/*gbk转为UTF8*/
std::string a2u(const std::string& s);
/*gbk转为UTF16*/
std::string a2l(const std::string& s);
/*utf16转为UTF8*/
std::string l2u(const std::string& s);
/*UTF8转为utf16*/
std::string u2l(const std::string& s);

#endif // _Convert2_H_