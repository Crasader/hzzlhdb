#include "cocos2d.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//////////////////////////////////////////////////////////////////////////
#include "../coPlatform.h"
#include "../../Libs/JsonAide.h"
#import "Pay.h"
#import "WXApiManager.h"
USING_NS_CC;
//---------------------------------------------------------------------------
std::string platformAction(const std::string& jsonString)
{
	JsonAide aide(jsonString);

	//判断读取成功与否
	if (aide.hasError())
	{
		CCLOG("--jniCcAction-----------\n%d\n----------------- <<%s>>", aide.getError(), __FUNCTION__);
		return "";
	}

	int act = aide.getInt("act", -1);
    switch (act) {
        case 300://支付宝支付
        {
            std::string propName = aide.getString("propName");
            std::string propInfo = aide.getString("propInfo");
            std::string propPice = aide.getString("propPice");
            
            CCLOG("%s %s  %s<<%s>>",propName.c_str(),propInfo.c_str(),propPice.c_str(),__PRETTY_FUNCTION__);
            
            Pay *pay=[[Pay alloc] init];
            [pay payAli:[NSString stringWithUTF8String:propName.c_str()] :[NSString stringWithUTF8String:propInfo.c_str()] :atof(propPice.c_str())];
            [pay autorelease];
            
            
        }
            break;
        case 400://微信支付
        {
            NSString *res = [WXApiManager jumpToBizPay];
            if( ![@"" isEqual:res] ){
                UIAlertView *alter = [[UIAlertView alloc] initWithTitle:@"支付失败" message:res delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                
                [alter show];
                [alter release];
            }
        }
            break;
        default:
            break;
    }
	CCLOG("ios.............................  %d",act);
	return "";
}
/*
 + (NSString *)jumpToBizPay {
 
 //============================================================
 // V3&V4支付流程实现
 // 注意:参数配置请查看服务器端Demo
 // 更新时间：2015年11月20日
 //============================================================
 NSString *urlString   = @"http://wxpay.weixin.qq.com/pub_v2/app/app_pay.php?plat=ios";
 //解析服务端返回json数据
 NSError *error;
 //加载一个NSURL对象
 NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:urlString]];
 //将请求的url数据放到NSData对象中
 NSData *response = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
 if ( response != nil) {
 NSMutableDictionary *dict = NULL;
 //IOS5自带解析类NSJSONSerialization从response中解析出数据放到字典中
 dict = [NSJSONSerialization JSONObjectWithData:response options:NSJSONReadingMutableLeaves error:&error];
 
 NSLog(@"url:%@",urlString);
 if(dict != nil){
 NSMutableString *retcode = [dict objectForKey:@"retcode"];
 if (retcode.intValue == 0){
 NSMutableString *stamp  = [dict objectForKey:@"timestamp"];
 
 //调起微信支付
 PayReq* req             = [[[PayReq alloc] init]autorelease];
 req.partnerId           = [dict objectForKey:@"partnerid"];
 req.prepayId            = [dict objectForKey:@"prepayid"];
 req.nonceStr            = [dict objectForKey:@"noncestr"];
 req.timeStamp           = stamp.intValue;
 req.package             = [dict objectForKey:@"package"];
 req.sign                = [dict objectForKey:@"sign"];
 [WXApi sendReq:req];
 //日志输出
 NSLog(@"appid=%@\npartid=%@\nprepayid=%@\nnoncestr=%@\ntimestamp=%ld\npackage=%@\nsign=%@",[dict objectForKey:@"appid"],req.partnerId,req.prepayId,req.nonceStr,(long)req.timeStamp,req.package,req.sign );
 return @"";
 }else{
 return [dict objectForKey:@"retmsg"];
 }
 }else{
 return @"服务器返回错误，未获取到json对象";
 }
 }else{
 return @"服务器返回错误";
 }

 */
//////////////////////////////////////////////////////////////////////////
#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
