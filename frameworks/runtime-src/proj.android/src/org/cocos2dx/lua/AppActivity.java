/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2016 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
 
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
package org.cocos2dx.lua;

import io.agora.agoracoco2dx.AgoraCoco2dx;
import android.os.Bundle;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import android.util.Log;

public class AppActivity extends Cocos2dxActivity{  
	

	private String TAG = "beck";
	private int targetSdkVersion = 0;
	private static final int PERMISSION_REQ_ID_RECORD_AUDIO  = 22;
	private static final int PERMISSION_REQ_ID_CAMERA  = 23;
	
	
	private static final String appId =  "You APP ID";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		AgoraCoco2dx rtcEngine = AgoraCoco2dx.getInstance();
		rtcEngine.init(getApplicationContext(), appId);


		try {
			final PackageInfo info = getPackageManager().getPackageInfo(getPackageName(), 0);
			targetSdkVersion = info.applicationInfo.targetSdkVersion;
			Log.d(TAG, "targetSdkVersion :" + targetSdkVersion);
		} catch (PackageManager.NameNotFoundException e) {
			e.printStackTrace();
		}
	
	
	}
}

