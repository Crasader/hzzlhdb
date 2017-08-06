
#ifndef _Convert2_H_
#define _Convert2_H_
#include <string>

/**
 * ����utf8�ַ�������
 * @param utf8 utf8�ַ��� 
 * @return �ַ�������
 */
int u8len(const std::string& str);

int u8cmp(const std::string& str1, const std::string& str2);

/**
 * ����ucs2�ַ�������
 * @param ucs2 ucs2�ַ��� 
 * @return �ַ�������
 */
int u2len(const std::string& s);

/*UTF8תΪgbk*/
std::string u2a(const std::string& s);
/*gbkתΪUTF8*/
std::string a2u(const std::string& s);
/*gbkתΪUTF16*/
std::string a2l(const std::string& s);
/*utf16תΪUTF8*/
std::string l2u(const std::string& s);
/*UTF8תΪutf16*/
std::string u2l(const std::string& s);

#endif // _Convert2_H_