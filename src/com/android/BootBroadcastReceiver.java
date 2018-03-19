/*************************************************************************
    > File Name: BootBroadcastReceiver.java
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Fri 17 Mar 2017 04:42:16 PM CST
 ************************************************************************/

package com.android;

import android.content.BroadcastReceiver;  
import android.content.Context;  
import android.content.Intent;  
import android.util.Log;

public class BootBroadcastReceiver extends BroadcastReceiver {
	@Override  
	public void onReceive(Context context, Intent intent) {
		Log.d("BootBroadcastReceiver", "=====开机广播.....");
		Intent service1 = new Intent(context,com.android.ril.RilService.class);  
		context.startService(service1);

		Intent service2 = new Intent(context,com.android.gameserver.GameService.class);  
		context.startService(service2);
	}
}

