package com.hzzgame.www.wxapi;
/**
 * Created by Ash on 2016/8/3.
 */

import com.hzzgame.www.Constants;
import com.tencent.mm.sdk.modelmsg.SendAuth;
import com.tencent.mm.sdk.openapi.*;
import com.tencent.mm.sdk.modelbase.*;
import org.cocos2dx.lib.Cocos2dxJavascriptJavaBridge;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

public class WXEntryActivity extends Activity implements IWXAPIEventHandler{

    private static final String APP_ID = "";
    private static WXEntryActivity app = null;
    private IWXAPI api;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        api = WXAPIFactory.createWXAPI(this, APP_ID, false);
        api.handleIntent(getIntent(), this);
        app = this;
    }

    // 微信发送请求到第三方应用时，会回调到该方法
    @Override
    public void onReq(BaseReq req) {

    }

    // 第三方应用发送到微信的请求处理后的响应结果，会回调到该方法
    @Override
    public void onResp(BaseResp resp) {
        //Toast.makeText(this, ((SendAuth.Resp)resp).code, Toast.LENGTH_LONG).show();
    	String errCode = "" + resp.errCode;
    	Log.i("onResp", errCode);
    	
    	//Constants.WX_CODE = ((SendAuth.Resp)resp).code;
		
    	switch (resp.errCode) {
		case BaseResp.ErrCode.ERR_OK:
			
			if(Constants.WX_index == 1)
			{
				Constants.WX_CODE = ((SendAuth.Resp)resp).code;
				
			}else
			{
				Constants.WX_fxcg = "1";	
			}
			
			//Constants.WX_CODE = ((SendAuth.Resp)resp).code;
			
			Constants.WX_index = 0;
			break;
		case BaseResp.ErrCode.ERR_USER_CANCEL:
			
			break;
		case BaseResp.ErrCode.ERR_AUTH_DENIED:
			
			break;
		default:
			
			break;
		}
    	
    	
        this.finish();
    }
}
