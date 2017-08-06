
#ifndef COMMON_UIHELPEREX_H_
#define COMMON_UIHELPEREX_H_

#include <string>
#include "cocos2d.h"

USING_NS_CC;

class HelperEx
{
public:
	static Node* seekNodeByTag(Node* root, int tag);
	static Node* seekNodeByName(Node* root, const std::string& name);
};

#endif // COMMON_UIHELPEREX_H_
