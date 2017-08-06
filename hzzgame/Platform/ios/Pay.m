//
//  Pay.m
//  PayPro
//
//  Created by 张 恒 on 16/4/14.
//
//

#import "Pay.h"
#import <AlipaySDK/AlipaySDK.h>
#import "Order.h"
#import "DataSigner.h"
@implementation Pay

#pragma mark -
#pragma mark   ==============产生随机订单号==============


- (NSString *)generateTradeNO
{
    static int kNumber = 15;
    
    NSString *sourceStr = @"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    NSMutableString *resultStr = [[NSMutableString alloc] init];
    srand((unsigned)time(0));
    for (int i = 0; i < kNumber; i++)
    {
        unsigned index = rand() % [sourceStr length];
        NSString *oneStr = [sourceStr substringWithRange:NSMakeRange(index, 1)];
        [resultStr appendString:oneStr];
    }
    return resultStr;
}
-(void)payAli:(NSString*) subject :(NSString *)body :(float)price{
    /*NSString *partner = @"2088121973004786";
    NSString *seller = @"3332433135@qq.com";
    NSString *privateKey = @"MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAN5WBj3H9o760AcWrXbvpbVcqbQhhsplsgjqcCgzmJ5adttILthE4rqmtR0flslj64w/kjksfJhFnCgnvvokhpBn+MKpmqMrZmZ2hgFc8otDp25hFHTAAfzmP9bvBaVKAzd4HZTNWp+LHpOZ70amB7kbadRt5eW1DCgpuPJiNRx9AgMBAAECgYBG+1nUwZFSxnpBuRi6Pvo9c47uZkG9PkhfllEmVIqmC6Wkjl/+RcfI5U4vNRWTyCW8SdNqE2zXxGl9XGsZOy4GNgOrBoyn41OhAkdO5YIt5gqalsLJYS74b1m/CANyYLgodFUlrQjczpxbJA6N+tKayw5y0hv8tZmOHCbopMDllQJBAPEOY3GPnirbZ5zOGJmiRVDhzco+4qwPa2ORFnDklqY4NMs/OPp6i7LSHPcxabcw2dxHh53vJ2ymH7Dt/uPWZ98CQQDsHor+inUdYBUCgdOvBsVEPa9aUE00n7P/exoVMWk6bY4Hn3suQSR0aEokmVoxmbZZGCuyW07Jhu/+FZbW7DcjAkAr2AIeVZDfGqjtzE6NIwaYf3l+n5F62hMeOPVr8yezc+WmUgl7BW8DR1QdYc9okmoNtrTj9Oumx1BcTe1eY1DhAkAwtAQbiNerXXw37iuJuK3L66VVzaZ/Kknocq3onS6FXL+lFuJDojAwyqowbmlKPdh8/B1XRrozm1+5Wx6n6kVdAkEAn8uyndfYne0UOzEWBkcNfwuTc+EN8WVWASKVcKIY1fd9RnqVNav6ypkX1gCeX3x1M895YzipuEwP/NB1RIxWBA==";*/
    NSString *partner = @"2088111930234071";
     NSString *seller = @"hzxiangwan@163.com";
     NSString *privateKey = @"MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAN64/lUSkVgd/cVhopVPrTquh+K0MvOt9k1boX3yBKmeB95ZV7y/GfiSAs9swuxZn7rkZN9WELqoB1P5lk/djECqWRmLBFOIetGqVZ3aqrVjVfHE3uYW3PgxCBXukMVzdX/7uzWHtyvsScFfo8LvKvchRhA4pVnlT11YMXMGuMdJAgMBAAECgYEAytXFYmrPjWV66Nh7PenbRJcQT+l2gbALoPXzRAU2M9clKV1KZy3PFC69enR9rMwOiSFZsH/sKmG+kr66IrAM+mB4QY1BtOzQXpWvzlXx8QMSbc6a1r/jHITpPKbd4gpU6f6OZVjkY1ViXN0q/mzt735hpP1tNTMbknpcrVAbqRECQQDwrvZhz0T/jr5vuvXKbpaVntN8h3aw3xHyARvWb8c+LPChwYlfHeLLRAJs8Zbl5EH7+YK5Idsz51DM9aTAzbbVAkEA7OVtOXMEfvUGYfBF15c21xFwwllTPbeJsa64YizRlSE32Z6oCiwkYD32kntKEt+0joVOPatmFsCVdql6MXIwpQJBAMRQAViwJfhIpt4dYcPWxe6OGLQXDeGgYVPCQcF7dMkrAK7/XBwAFW90LoJL36ftsUBkOJLmoZPJVeeHaPV2FW0CQGiVfRwAyDpYaEHytpLQYliejX5vuw0jKznzXdmR2dERwjtkDOG4zTJkCKhwn6oncun8ticJtV9UFGg3uAd+VzUCQAk/iafjGsJS5drAHvvBYeFQR5ooG/CqMz2K4MHnHGh/pAWCLnNQgO6FgNl7yjkjuMgbRBskyMwdN/3atqT0RsQ=";
    /*============================================================================*/
    /*============================================================================*/
    /*============================================================================*/
    
    
    /*
     *生成订单信息及签名
     */
    //将商品信息赋予AlixPayOrder的成员变量
    Order *order = [[Order alloc] init];
    order.partner = partner;
    order.seller = seller;
    order.tradeNO = [self generateTradeNO]; //订单ID（由商家自行制定）
    order.productName = subject; //商品标题
    order.productDescription = body; //商品描述
    order.amount = [NSString stringWithFormat:@"%.2f",price]; //商品价格
    order.notifyURL =  @"http://www.xxx.com"; //回调URL
    
    order.service = @"mobile.securitypay.pay";
    order.paymentType = @"1";
    order.inputCharset = @"utf-8";
    order.itBPay = @"30m";
    order.showUrl = @"m.alipay.com";
    
    //应用注册scheme,在AlixPayDemo-Info.plist定义URL types
    NSString *appScheme = @"payProXW";
    
    //将商品信息拼接成字符串
    NSString *orderSpec = [order description];
    NSLog(@"orderSpec = %@",orderSpec);
    
    //获取私钥并将商户信息签名,外部商户可以根据情况存放私钥和签名,只需要遵循RSA签名规范,并将签名字符串base64编码和UrlEncode
    id<DataSigner> signer = CreateRSADataSigner(privateKey);
    NSString *signedString = [signer signString:orderSpec];
    
    //将签名成功字符串格式化为订单字符串,请严格按照该格式
    NSString *orderString = nil;
    if (signedString != nil) {
        orderString = [NSString stringWithFormat:@"%@&sign=\"%@\"&sign_type=\"%@\"",
                       orderSpec, signedString, @"RSA"];
        
        [[AlipaySDK defaultService] payOrder:orderString fromScheme:appScheme callback:^(NSDictionary *resultDic) {
            int resultStatus=[[resultDic objectForKey:@"resultStatus"] intValue];
            //NSLog(@"reslut = %@",resultDic);
            switch (resultStatus) {
                case 6001:
                    NSLog(@"用户取消支付");
                    break;
                case 6002:
                    NSLog(@"网络连接出错");
                    break;
                case 4000:
                    NSLog(@"订单支付失败");
                    break;
                case 8000:
                    NSLog(@"正在处理中");
                    break;
                case 9000:
                    NSLog(@"订单支付成功");
                    break;
                default:
                    break;
            }
        }];
    }
    
}

@end
