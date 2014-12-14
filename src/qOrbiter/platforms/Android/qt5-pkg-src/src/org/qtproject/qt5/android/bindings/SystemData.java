package org.qtproject.qt5.android.bindings;


import android.os.Build;
import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.Build.VERSION;


import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.lang.String;

public class SystemData {

    private static final String TAG="SystemData";

      public  String getStorageLocation(){
          Log.d(TAG, "Getting storage location information");
        File path = Environment.getExternalStorageDirectory();
        return path.toString();
    }

    public  boolean getActivated(){
        return true;
    }





}

