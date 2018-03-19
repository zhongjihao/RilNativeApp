package com.android.gameserver;

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
import java.io.InputStream;
import java.io.IOException;
import java.io.BufferedWriter;  
import java.io.File;  
import java.io.FileWriter;
import com.android.gameserver.GameJni;


public class GameService extends Service{
	private final static String TAG = "GameService";
	private Context context;
	private boolean isServiceRunning = false;

    /** Called when the service is first created. */
    @Override
    public void onCreate(){
        super.onCreate();
		Log.d(TAG, "====Game====onCreate======");
		context = this;
		isServiceRunning = true;
		GameJni.OnGamePlazaStart();

		GameJni.OnGameRoom1Start(GameJni.Gold1ServerIP,GameJni.Gold1ServerPort,GameJni.Gold1ServerID ,2);
		GameJni.OnGameRoom2Start(GameJni.Gold2ServerIP,GameJni.Gold2ServerPort,GameJni.Gold2ServerID,2);
		GameJni.OnGameRoom3Start(GameJni.Score1ServerIP,GameJni.Score1ServerPort,GameJni.Score1ServerID,1);
		GameJni.OnGameRoom4Start(GameJni.Score2ServerIP,GameJni.Score2ServerPort,GameJni.Score2ServerID,1);

        IntentFilter filter = new IntentFilter(Intent.ACTION_TIME_TICK);   
		ListenBroadcast receiver = new ListenBroadcast();   
		registerReceiver(receiver, filter);

		new Thread(new Runnable() {
			@Override
			public void run() {
				try{
					InputStream is = getAssets().open("RoomConfig.ini");
                    int size = is.available();
                    byte[] buffer = new byte[size];  
		            is.read(buffer);  
		            is.close();

					String path = getApplicationContext().getFilesDir().getAbsolutePath();

		            Log.d(TAG, "====Game====onCreate======path: "+path);
                    File file = new File(path+"/RoomConfig.ini");
                    if(!file.exists()) {
						file.createNewFile();
					}
                    
					GameJni.setGameConfigIni(file.getAbsolutePath());	
					FileWriter fw = new FileWriter(file.getAbsoluteFile());
					BufferedWriter bw = new BufferedWriter(fw);
					bw.write(new String(buffer));
				    bw.close();

				}catch (Exception e){

				}
			}
		}).start();
    }

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override  
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d(TAG, "====Game====onStartCommand========");
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy(){
		Log.d(TAG,"===Game=====onDestroy=====");
		isServiceRunning = false;
	}

	class ListenBroadcast extends BroadcastReceiver {
		@Override  
		public void onReceive(Context context, Intent intent) {
		//	Log.d(TAG, "===Game=====1===ListenBroadcast start .....");
            if (intent.getAction().equals(Intent.ACTION_TIME_TICK)) {
			//	Log.d(TAG, "===Game======2====ListenBroadcast start .....");
				//检查Service状态
				ActivityManager manager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
				for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
					if("com.android.gameserver.GameService".equals(service.service.getClassName())){
						isServiceRunning = true;
					}
				}
		//		Log.d(TAG, "====Game===3===ListenBroadcast start===isServiceRunning: "+isServiceRunning);
				if (!isServiceRunning) {
					 Intent i = new Intent(context, GameService.class);
					 context.startService(i);
				}
			}
		}
	}
}

