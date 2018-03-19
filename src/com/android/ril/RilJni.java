package com.android.ril;

import com.android.ril.RilCallback;
import android.util.Log;

public final class RilJni{
	private final static String TAG = "RilJni";
    private static String RilSocketName = "localsocketrild";

	static{
		Log.d(TAG, "==Rild==loadLibrary==start===");	
		System.loadLibrary("ril_native");
		Log.d(TAG, "==Rild===loadLibrary==end===");	
	}

    public static native void OnRilStart(RilCallback inter);
}

