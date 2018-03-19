/*************************************************************************
    > File Name: MyApplication.java
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: Mon 20 Mar 2017 08:43:21 PM CST
 ************************************************************************/

package com.android;

import android.app.Application;
import android.util.Log;

public class MyApplication extends Application {
	 public final static String TAG ="MyApplication";
	 private static MyApplication singleton;
	 
	 public static MyApplication getInstance(){
		 return singleton;  
	 }
	 
	 @Override
	 public final void onCreate() {
		 super.onCreate();
		 singleton = this;  
	 }
	
 }

