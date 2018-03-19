package com.android.gameserver;

import android.util.Log;

public final class GameJni{
	private final static String TAG = "GameJni";
	//游戏大厅服务器IP和端口
    private static String PlazaServerIP = "127.0.0.1";
	private static int PlazaServerPort = 8888;

	//金币场房间1	
    public static String Gold1ServerIP = "127.0.0.1";
	public static int Gold1ServerPort = 9999;
	public static int Gold1ServerID = 2012;

	//金币场房间2	
    public static String Gold2ServerIP = "127.0.0.1";
	public static int Gold2ServerPort = 9998;
	public static int Gold2ServerID = 2013;

    //积分场房间1	
    public static String Score1ServerIP = "127.0.0.1";
	public static int Score1ServerPort = 10010;
	public static int Score1ServerID = 2014;

	//积分场房间2	
    public static String Score2ServerIP = "127.0.0.1";
	public static int Score2ServerPort = 10011;
	public static int Score2ServerID = 2015;

	static{
		Log.d(TAG, "==Game====loadLibrary==start===");	
		System.loadLibrary("gameserver");
		Log.d(TAG, "==Game===loadLibrary==end===");	
	}

    public static native void OnGamePlazaStart();
    public static native void OnGameRoom1Start(String ip,int port,int wServerId,int wServerType);
    public static native void OnGameRoom2Start(String ip,int port,int wServerId,int wServerType);
    public static native void OnGameRoom3Start(String ip,int port,int wServerId,int wServerType);
    public static native void OnGameRoom4Start(String ip,int port,int wServerId,int wServerType);
	public static native void setGameConfigIni(String filepath);
}

