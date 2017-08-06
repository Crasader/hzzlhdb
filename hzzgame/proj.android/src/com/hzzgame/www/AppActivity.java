/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package com.hzzgame.www;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.Random;

import com.hzzgame.www.wxapi.SendToWXActivity;


import org.cocos2dx.lib.Cocos2dxActivity;
import org.json.JSONObject;
import org.json.JSONTokener;

import com.alipay.sdk.app.PayTask;
import com.tencent.mm.sdk.constants.Build;
import com.tencent.mm.sdk.constants.ConstantsAPI;
import com.tencent.mm.sdk.modelbase.BaseReq;
import com.tencent.mm.sdk.modelbase.BaseResp;
import com.tencent.mm.sdk.modelmsg.SendAuth;
import com.tencent.mm.sdk.modelpay.PayReq;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;
import com.tencent.mm.sdk.openapi.IWXAPIEventHandler;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;


public class AppActivity extends Cocos2dxActivity{
	private BroadcastReceiver wxAppRegisterReceiver;//微信注册
	public static AppActivity game;
	private int iActionType = 0;
	
	private static final int SDK_PAY_FLAG = 1;
	private static final int SDK_CHECK_FLAG = 2;
	private static final int WX_PAY_FLAG=3;
	//private String sOrderId;//订单号
	// 商户PID
	public static final String PARTNER = "";
	// 商户收款账号
	public static final String SELLER = "";
	// 商户私钥，pkcs8格式
	public static final String RSA_PRIVATE = 
			"";
	
	// IWXAPI 是第三方app和微信通信的openapi接口
    private IWXAPI api;
    
    //微信支付信息
    private String fee_;
    private String accounts_;
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		game = this;

		api = WXAPIFactory.createWXAPI(this, "",true);
		api.registerApp("");
	}
	public static AppActivity shared() {
		return game;
	}
	public String jniCjAction(String str) {
		String code = "";
		try {
			JSONTokener jsonTokener = new JSONTokener(str);
			JSONObject jsonObject = (JSONObject) jsonTokener.nextValue();
			iActionType = jsonObject.getInt("act");
			switch (iActionType) {
			case 300:// 支付
			{
				final String propName = jsonObject.getString("propName");
				final String propInfo = jsonObject.getString("propInfo");
				final String propPice = jsonObject.getString("propPice");
				System.out.println(propName + propInfo + propPice);
				pay(propName,propInfo,propPice);
			}
				break;
			case 400:
			{
				accounts_ = jsonObject.getString("propInfo");
				fee_ = jsonObject.getString("propPice");
				payWX();
				/**/
			}
				break;
			case 500://分享
			{
				Constants.WX_index = 2;
				SendToWXActivity pSendActivity=new SendToWXActivity();
				pSendActivity.sendShareTest("红蜘蛛游戏中心邀您疯狂捕鱼勇斗牛牛！",1);
			}
			break;
			case 501://分享
			{
				Constants.WX_index = 2;
				SendToWXActivity pSendActivity=new SendToWXActivity();
				pSendActivity.sendShareTest("红蜘蛛游戏中心邀您疯狂捕鱼勇斗牛牛！",0);
			}
			break;
			case 5001://分享
			{
				final String neirong = jsonObject.getString("neirong");
				Constants.WX_index = 2;
				SendToWXActivity pSendActivity=new SendToWXActivity();
				pSendActivity.sendShareTest_pq(neirong);
			}
			break;
			case 5011://分享
			{
				final String neirong = jsonObject.getString("neirong");
				Constants.WX_index = 2;
				SendToWXActivity pSendActivity=new SendToWXActivity();
				pSendActivity.sendShareTest_py(neirong);
			}
			break;
			case 600://WX
			{
				Constants.WX_index = 1;
				WXLogin();
			}
			break;
			
			case 601://WX Code
			{
				code = Constants.WX_CODE;
			}
			break;
			
			case 602://WX init Code
			{
				Constants.WX_CODE = "0";
			}
			break;
			
			case 603://WX APPID
			{
				code = Constants.APP_ID;
			}
			break;
			
			case 604://WX APPSecret
			{
				code = Constants.APP_Secret;
			}
			break;	
			case 701:
			{
				code = Constants.WX_fxcg;
				Constants.WX_fxcg = "0";
			}
			break;
			default:
				break;
			}

		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return code;
	}
	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();
		//微信注册
				wxAppRegisterReceiver=new AppRegister();
				IntentFilter filterAppRegister = new IntentFilter();
				filterAppRegister.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
				registerReceiver(wxAppRegisterReceiver, filterAppRegister);
	}
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		unregisterReceiver(wxAppRegisterReceiver);
	}
	
	protected void WXLogin(){
		Log.i("WXLogin_GET", "WXLogin");
		final SendAuth.Req req = new SendAuth.Req();
		req.scope = "snsapi_userinfo";
		req.state = "wechat_login";
		api.sendReq(req);
	}
	
	private void payWX(){Log.i("PAY_GET", "payWX");
		Runnable payRunnable = new Runnable() {
			
			@Override
			public void run() {
//String url = "http://wxpay.weixin.qq.com/pub_v2/app/app_pay.php?plat=android";
String url = "http://wxch.hzzgame.com/App_pay.aspx";
// 签约卖家支付宝账号
url += "?fee="  + fee_ ;
// 商户网站唯一订单号
url += "&body="  + accounts_ ;
//Toast.makeText(AppActivity.this, "获取订单中...", Toast.LENGTH_SHORT).show();
//url = "http://www.hzzgame.com:8098/App_pay.aspx?fee=2&body=slc04";
				//Toast.makeText(AppActivity.this, "获取订单中...", Toast.LENGTH_SHORT).show();
		        try{
					byte[] buf = Util.httpGet(url);
					if (buf != null && buf.length > 0) {
						String content = new String(buf);
						Log.e("get server pay params:",content);
			        	JSONObject json = new JSONObject(content); 
						if(null != json && !json.has("retcode") ){
							PayReq req = new PayReq();
							req.appId			= json.getString("appid");
							req.partnerId		= json.getString("partnerid");
							req.prepayId		= json.getString("prepayid");
							req.nonceStr		= json.getString("noncestr");
							req.timeStamp		= json.getString("timestamp");
							req.packageValue	= json.getString("package");
							req.sign			= json.getString("sign");
							req.extData			= "app data"; // optional
							//Toast.makeText(AppActivity.this, "正常调起支付", Toast.LENGTH_SHORT).show();
							// 在支付之前，如果应用没有注册到微信，应该先调用IWXMsg.registerApp将应用注册到微信
							api.sendReq(req);
							
						}else{
				        	Log.d("PAY_GET", "返回错误"+json.getString("retmsg"));
				        	Toast.makeText(AppActivity.this, "返回错误"+json.getString("retmsg"), Toast.LENGTH_SHORT).show();
						}
					}else{
			        	Log.d("PAY_GET", "服务器请求错误");
			        	//Toast.makeText(AppActivity.this, "服务器请求错误", Toast.LENGTH_SHORT).show();
			        }
		        }catch(Exception e){
		        	Log.e("PAY_GET", "异常："+e.getMessage());
		        	//Toast.makeText(AppActivity.this, "异常："+e.getMessage(), Toast.LENGTH_SHORT).show();
		        }
				// 构造PayTask 对象
				//PayTask alipay = new PayTask(AppActivity.this);
				// 调用支付接口，获取支付结果
				//String result = alipay.pay(payInfo,true);
				boolean isPaySupported = api.getWXAppSupportAPI() >= Build.PAY_SUPPORTED_SDK_INT;
				
				Message msg = new Message();
				msg.what = WX_PAY_FLAG;
				msg.obj = isPaySupported;
				mHandler.sendMessage(msg);
			}
		};

		// 必须异步调用
		Thread payThread = new Thread(payRunnable);
		payThread.start();
	}
	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case SDK_PAY_FLAG: {
				PayResult payResult = new PayResult((String) msg.obj);

				// 支付宝返回此次支付结果及加签，建议对支付宝签名信息拿签约时支付宝提供的公钥做验签
				String resultInfo = payResult.getResult();

				String resultStatus = payResult.getResultStatus();

				// 判断resultStatus 为“9000”则代表支付成功，具体状态码代表含义可参考接口文档
				if (TextUtils.equals(resultStatus, "9000")) {
					Toast.makeText(AppActivity.this, "支付成功",
							Toast.LENGTH_SHORT).show();
				} else {
					// 判断resultStatus 为非“9000”则代表可能支付失败
					// “8000”代表支付结果因为支付渠道原因或者系统原因还在等待支付结果确认，最终交易是否成功以服务端异步通知为准（小概率状态）
					if (TextUtils.equals(resultStatus, "8000")) {
						Toast.makeText(AppActivity.this, "支付结果确认中",
								Toast.LENGTH_SHORT).show();

					} else {
						// 其他值就可以判断为支付失败，包括用户主动取消支付，或者系统返回的错误
						Toast.makeText(AppActivity.this, "支付失败" + resultStatus,
								Toast.LENGTH_SHORT).show();

					}
				}
				break;
			}
			case SDK_CHECK_FLAG: {
				Toast.makeText(AppActivity.this, "检查结果为：" + msg.obj,
						Toast.LENGTH_SHORT).show();
				break;
			}
			case WX_PAY_FLAG:{
				//Toast.makeText(AppActivity.this, String.valueOf(msg.obj), Toast.LENGTH_SHORT).show();
				break;
			}
			default:
				break;
			}
		};
	};
	/**
	 * call alipay sdk pay. 调用SDK支付
	 * 
	 */
	public void pay(String propName,String propContent,String price) {
		// 订单
		String orderInfo = getOrderInfo(propName, propContent, price);

		// 对订单做RSA 签名
		String sign = sign(orderInfo);
		try {
			// 仅需对sign 做URL编码
			sign = URLEncoder.encode(sign, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}

		// 完整的符合支付宝参数规范的订单信息
		final String payInfo = orderInfo + "&sign=\"" + sign + "\"&"
				+ getSignType();

		Runnable payRunnable = new Runnable() {

			@Override
			public void run() {
				// 构造PayTask 对象
				PayTask alipay = new PayTask(AppActivity.this);
				// 调用支付接口，获取支付结果
				String result = alipay.pay(payInfo,true);

				Message msg = new Message();
				msg.what = SDK_PAY_FLAG;
				msg.obj = result;
				mHandler.sendMessage(msg);
			}
		};

		// 必须异步调用
		Thread payThread = new Thread(payRunnable);
		payThread.start();
	}
	/**
	 * create the order info. 创建订单信息
	 * 
	 */
	public String getOrderInfo(String subject, String body, String price) {
		// 签约合作者身份ID
		String orderInfo = "partner=" + "\"" + PARTNER + "\"";
		// 签约卖家支付宝账号
		orderInfo += "&seller_id=" + "\"" + SELLER + "\"";
		// 商户网站唯一订单号
		orderInfo += "&out_trade_no=" + "\"" + getOutTradeNo() + "\"";
		
		//orderInfo += "&out_trade_no=" + "\"" + sOrderId + "\"";
		// 商品名称
		orderInfo += "&subject=" + "\"" + subject + "\"";
		// 商品详情
		orderInfo += "&body=" + "\"" + body + "\"";
		// 商品金额
		orderInfo += "&total_fee=" + "\"" + price + "\"";
		// 服务器异步通知页面路径
		// orderInfo += "&notify_url=" + "\"" +
		// "http://pay.999qp.com/alipay/notify_url.aspx"
		orderInfo += "&notify_url=" + "\"" + "http://zfb.hzzgame.com/notify_url.aspx" + "\"";
		
		// 服务接口名称， 固定值
		orderInfo += "&service=\"mobile.securitypay.pay\"";
		// 支付类型， 固定值
		orderInfo += "&payment_type=\"1\"";
		// 参数编码， 固定值
		orderInfo += "&_input_charset=\"utf-8\"";
		// 设置未付款交易的超时时间
		// 默认30分钟，一旦超时，该笔交易就会自动被关闭。
		// 取值范围：1m～15d。
		// m-分钟，h-小时，d-天，1c-当天（无论交易何时创建，都在0点关闭）。
		// 该参数数值不接受小数点，如1.5h，可转换为90m。
		orderInfo += "&it_b_pay=\"30m\"";
		// extern_token为经过快登授权获取到的alipay_open_id,带上此参数用户将使用授权的账户进行支付
		// orderInfo += "&extern_token=" + "\"" + extern_token + "\"";
		// 支付宝处理完请求后，当前页面跳转到商户指定页面的路径，可空
		// orderInfo +=
		// "&return_url=\"http://pay.999qp.com/alipay/return_url.aspx\"";
		orderInfo += "&return_url=\"m.alipay.com\"";
		//orderInfo += "&return_url=\"http://pay.qicainiu.com/alipay/return_url.aspx\"";
		// 调用银行卡支付，需配置此参数，参与签名， 固定值 （需要签约《无线银行卡快捷支付》才能使用）
		// orderInfo += "&paymethod=\"expressGateway\"";
		return orderInfo;
	}

	/**
	 * sign the order info. 对订单信息进行签名
	 * 
	 * @param content
	 *            待签名订单信息
	 */
	public String sign(String content) {
		return SignUtils.sign(content, RSA_PRIVATE);
	}

	/**
	 * get the sign type we use. 获取签名方式
	 * 
	 */
	public String getSignType() {
		return "sign_type=\"RSA\"";
	}

	/**
	 * get the out_trade_no for an order. 生成商户订单号，该值在商户端应保持唯一（可自定义格式规范）
	 * 
	 */
	public String getOutTradeNo() {
		SimpleDateFormat format = new SimpleDateFormat("MMddHHmmss", Locale.getDefault());
		Date date = new Date();
		String key = format.format(date);

		Random r = new Random();
		key = key + r.nextInt();
		key = key.substring(0, 15);
		return key;
	}
	/***
	 * 
	 * @param prefix
	 * @return
	 */
	public static String GetOrderIDByPrefix(String prefix)
    {
        //构造订单号 (形如:20101201102322159111111)
        int orderIDLength = 32;
        int randomLength = 6;
        StringBuffer tradeNoBuffer = new StringBuffer();

        tradeNoBuffer.append(prefix);
        tradeNoBuffer.append(GetDateTimeLongString());

        if ((tradeNoBuffer.length() + randomLength) > orderIDLength)
            randomLength = orderIDLength - tradeNoBuffer.length();

        tradeNoBuffer.append(CreateRandom(randomLength, 1, 0, 0, 0, ""));
        System.out.println("toString:"+tradeNoBuffer.toString());
        return tradeNoBuffer.toString();
    }

	 public static String GetDateTimeLongString()
     {
		SimpleDateFormat format = new SimpleDateFormat("yyyyMMddHHmmssSSS",
					Locale.getDefault());
		Date date = new Date();
		String key = format.format(date);
		return key;
     }
public static String CreateRandom(int length, int useNum, int useLow, int useUpp, int useSpe, String custom)
    {
        byte[] data = new byte[4];
       // new RNGCryptoServiceProvider().GetBytes(data);
        //Random random = new Random(BitConverter.ToInt32(data, 0));
        Random random=new Random();
        String str = "";
        String str2 = custom;
        if (useNum == 1)
        {
            str2 = str2 + "0123456789"; 
        }
        if (useLow == 1)
        {
            str2 = str2 + "abcdefghijklmnopqrstuvwxyz";
        }
        if (useUpp == 1)
        {
            str2 = str2 + "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        }
        if (useSpe == 1)
        {
            str2 = str2 + "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
        }
        for (int i = 0; i < length; i++)
        {
        	int start=random.nextInt(str2.length() - 1);
        	String strTemp= str2.substring(start, start+1);
            str = str +strTemp;
        }
        return str;
    }
}
