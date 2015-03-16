package org.qtproject.qt5.android.bindings;


import android.content.res.AssetManager;
import android.os.Build;
import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.Build.VERSION;


import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
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

        public void moveFiles() {

                File start = new File("/data/data/org.linuxmce.QOrbiterQt5/qt-reserved-files/qml");

                File target = new File(Environment.getExternalStorageDirectory()+"/LinuxMCE/splash");


                // get a listing of all files in the directory
                String[] filesInDir = start.list();

                // sort the list of files (optional)
                // Arrays.sort(filesInDir);

                // have everything i need, just print it now
                for ( int i=0; i<filesInDir.length; i++ )
                {
                 Log.d(TAG, "file: " + filesInDir[i] );
                }
                if(start.exists()){
                        try {
                                copyDirectoryOneLocationToAnotherLocation(start, target);
                        } catch (IOException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                        }
                    } else {
                         Log.d(TAG, "Could not find assets folder!");
                         Log.d(TAG, start.listFiles().toString());
                    }

                File target2 = new File(Environment.getExternalStorageDirectory()+"/LinuxMCE/skins" );
                File start2 =  new File("/data/data/org.linuxmce.QOrbiterQt5/qt-reserved-files/qml/android-build/assets/skins");
                if(start2.exists()){
                        try {
                                copyDirectoryOneLocationToAnotherLocation(start2, target2);
                        } catch (IOException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                        }
                }

        }

        public static void copyDirectoryOneLocationToAnotherLocation(File sourceLocation, File targetLocation)
                        throws IOException {

                if (sourceLocation.isDirectory()) {
                        if (!targetLocation.exists()) {
                                targetLocation.mkdir();
                        }

                        String[] children = sourceLocation.list();
                        for (int i = 0; i < sourceLocation.listFiles().length; i++) {

                                copyDirectoryOneLocationToAnotherLocation(new File(sourceLocation, children[i]),
                                                new File(targetLocation, children[i]));
                        }
                } else {

                        InputStream in = new FileInputStream(sourceLocation);

                        OutputStream out = new FileOutputStream(targetLocation);

                        // Copy the bits from instream to outstream
                        byte[] buf = new byte[1024];
                        int len;
                        while ((len = in.read(buf)) > 0) {
                                out.write(buf, 0, len);
                        }
                        in.close();
                        out.close();
                }

        }

        public static void deleteRecursive(File fileOrDirectory) {

                if (fileOrDirectory.isDirectory())
                        for (File child : fileOrDirectory.listFiles())
                                deleteRecursive(child);

                fileOrDirectory.delete();

        }

        public static void clearFiles() {
                File splash = new File("/data/data/org.linuxmce.QOrbiterQt5/qt-reserved-files/qml/android-build/assets/splash");
                File skins =  new File("/data/data/org.linuxmce.QOrbiterQt5/qt-reserved-files/qml/android-build/assets/skins");

                deleteRecursive(splash);
                deleteRecursive(skins);
        }


}

