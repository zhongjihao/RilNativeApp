package com.android.ril;

import android.app.Service;
import android.os.Bundle;
import android.os.IBinder;
import android.content.Intent;
import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.Context;
import android.content.IntentFilter;
import android.util.Log;
import com.android.ril.RilJni;
import com.android.ril.RilCallback;


public class RilService extends Service implements RilCallback{
	private final static String TAG = "RilService";
	private Context context;
	private boolean isServiceRunning = false;

    /** Called when the service is first created. */
    @Override
    public void onCreate(){
        super.onCreate();
		Log.d(TAG, "====Rild====onCreate======");
		context = this;
		isServiceRunning = true;
		RilJni.OnRilStart(this);
        IntentFilter filter = new IntentFilter(Intent.ACTION_TIME_TICK);   
		ListenBroadcast receiver = new ListenBroadcast();   
		registerReceiver(receiver, filter);
    }

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override  
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d(TAG, "====Rild=====onStartCommand========");
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy(){
		Log.d(TAG,"===Rild=====onDestroy=====");
		isServiceRunning = false;
	}

	class ListenBroadcast extends BroadcastReceiver {
		@Override  
		public void onReceive(Context context, Intent intent) {
			Log.d(TAG, "==Rild===1===ListenBroadcast start .....");
            if (intent.getAction().equals(Intent.ACTION_TIME_TICK)) {
				Log.d(TAG, "==Rild====2====ListenBroadcast start .....");
				//检查Service状态
				ActivityManager manager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
				for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
					if("com.android.ril.RilService".equals(service.service.getClassName())){
						isServiceRunning = true;
					}
				}
				Log.d(TAG, "==Rild==3===ListenBroadcast start===isServiceRunning: "+isServiceRunning);
				if (!isServiceRunning) {
					 Intent i = new Intent(context, RilService.class);
					 context.startService(i);
				}
			}
		}
	}
   

	@Override
	public void RIL_onRequestThreadQuit(){
		Log.d(TAG, "==Rild========RIL_onRequestThreadQuit======");
		Intent i = new Intent(context, RilService.class);
		context.stopService(i); 
	}
}

