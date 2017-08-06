package com.hzzgame.www.wxapi;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import com.hzzgame.www.AppActivity;
import com.hzzgame.www.Constants;
import com.hzzgame.www.R;

import com.tencent.mm.sdk.modelmsg.SendMessageToWX;
import com.tencent.mm.sdk.modelmsg.WXMediaMessage;
import com.tencent.mm.sdk.modelmsg.WXWebpageObject;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;
public class SendToWXActivity {

	public void sendShareTest(String text, int flag){
		
		 WXWebpageObject webpage = new WXWebpageObject();  
		 webpage.webpageUrl = "http://fenxiang.hzzgame.com";
		 WXMediaMessage msg = new WXMediaMessage(webpage);  
		 msg.title = (String) AppActivity.game.getResources().getText(R.string.app_name);
		 msg.description = text;  
		 //这里替换一张自己工程里的图片资源  
		 Bitmap thumb = BitmapFactory.decodeResource(AppActivity.game.getResources(), R.drawable.fenxiang);  
		 msg.setThumbImage(thumb);        
		 SendMessageToWX.Req req = new SendMessageToWX.Req();  
		 req.transaction = String.valueOf(System.currentTimeMillis());  
		 req.message = msg;  
		
		 req.scene = flag==0?SendMessageToWX.Req.WXSceneSession:SendMessageToWX.Req.WXSceneTimeline;  
		 IWXAPI api= WXAPIFactory.createWXAPI(AppActivity.game, Constants.APP_ID);
		 api.sendReq(req);
	}
	
	public void sendShareTest_py(String text){
		
		 WXWebpageObject webpage = new WXWebpageObject();  
		 webpage.webpageUrl = "http://fenxiang.hzzgame.com";
		 WXMediaMessage msg = new WXMediaMessage(webpage);  
		 msg.title = (String) AppActivity.game.getResources().getText(R.string.app_name);
		 msg.description = text;  
		 //这里替换一张自己工程里的图片资源  
		 Bitmap thumb = BitmapFactory.decodeResource(AppActivity.game.getResources(), R.drawable.fenxiang);  
		 msg.setThumbImage(thumb);        
		 SendMessageToWX.Req req = new SendMessageToWX.Req();  
		 req.transaction = String.valueOf(System.currentTimeMillis());  
		 req.message = msg;  
		
		 req.scene = SendMessageToWX.Req.WXSceneSession;  
		 IWXAPI api= WXAPIFactory.createWXAPI(AppActivity.game, Constants.APP_ID);
		 api.sendReq(req);
	}
	
	public void sendShareTest_pq(String text){
		
		 WXWebpageObject webpage = new WXWebpageObject();  
		 webpage.webpageUrl = "http://fenxiang.hzzgame.com";
		 WXMediaMessage msg = new WXMediaMessage(webpage);  
		 msg.title = text;
		 msg.description =(String) AppActivity.game.getResources().getText(R.string.app_name);  
		 //这里替换一张自己工程里的图片资源  
		 Bitmap thumb = BitmapFactory.decodeResource(AppActivity.game.getResources(), R.drawable.fenxiang);  
		 msg.setThumbImage(thumb);        
		 SendMessageToWX.Req req = new SendMessageToWX.Req();  
		 req.transaction = String.valueOf(System.currentTimeMillis());  
		 req.message = msg;  
		
		 req.scene = SendMessageToWX.Req.WXSceneTimeline;  
		 IWXAPI api= WXAPIFactory.createWXAPI(AppActivity.game, Constants.APP_ID);
		 api.sendReq(req);
	}

	/*private String buildTransaction(final String type) {
		return (type == null) ? String.valueOf(System.currentTimeMillis()) : type + System.currentTimeMillis();
	}*/
}
