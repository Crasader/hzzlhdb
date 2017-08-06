#include "cocos2d.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//////////////////////////////////////////////////////////////////////////

#include "../coPlatform.h"
#include "../../base/JsonAide.h"
#include "JniHelper.h"
USING_NS_CC;

/**
 * 执行动作
 * @param jsonString 动作参数 {act:0, ...}
 */
std::string platformAction(const std::string& jsonString)
{
	JsonAide aide(jsonString);

	//判断读取成功与否
	if (aide.hasError())
	{
		CCLOG("--jniCcAction-----------\n%s\n----------------- <<%s>>", aide.getError(), __FUNCTION__);
		return false;
	}

	int act = aide.getInt("act", -1);

	CCLOG("android.............................  %d",act);
		JniMethodInfo minfo;
		jobject jobj;//存对象
		if (getJniMethod(jobj, minfo, "jniCjAction","(Ljava/lang/String;)Ljava/lang/String;"))
		{
		jstring sJson	= minfo.env->NewStringUTF(jsonString.c_str());
		//调用openWebview, 参数1：Test对象   参数2：方法ID
		jstring result = (jstring)minfo.env->CallObjectMethod(jobj, minfo.methodID, sJson);
		const char* str = minfo.env->GetStringUTFChars(result, 0);
		std::string retUtf8 = str;
		//删除字符串
		//if (retUtf8.length()>0)
		{
			minfo.env->ReleaseStringUTFChars(result, str);
			minfo.env->DeleteLocalRef (sJson);
		}
		return retUtf8;
		}

	
	return "";
}
//////////////////////////////////////////////////////////////////////////
#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
