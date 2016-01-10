/**
*
*/
package org.kde.necessitas.origo;


import android.os.Binder;
import android.os.StrictMode;
import android.os.SystemClock;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.media.MediaPlayer;
import android.util.Log;
import java.io.IOException;
import java.lang.IllegalStateException;
import java.lang.NullPointerException;

import java.io.ObjectOutputStream;

import android.media.AudioManager;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnPreparedListener;


import android.widget.Toast;
import java.net.Socket;
import java.net.SocketException;

import android.os.Handler;
import android.content.Context;
import android.media.AudioManager;

/**
* @author langston
* The Idea behind this class is that it acts as a service to
*  *Play linuxmce Audio media
*  *Report back to linuxmce relevant information like track started/stopped, timecode, etc
*  We are using a local server to talk back to the Qt application.
*
*/
public class LinuxmceAudioService extends Service implements
OnBufferingUpdateListener, OnErrorListener, OnCompletionListener, 
OnPreparedListener {


	private final IBinder mBinder = new LocalBinder();
        private static final String TAG = "LinuxMCE Media Service";
        private final Handler handler = new Handler();
        private MediaPlayer mp;
        private String current;

	/** This runnable is executed initially by a handler, but later internally the handler restarts it while the 
        * media player is playing.
        */
        public class TimecodeRunnable implements Runnable {
        private String message;

		public void run() {
                if(mp == null || !mp.isPlaying()){
                return;
                }
                android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);

			message="|"+mp.getCurrentPosition()+"|" ;
                        //Log.d(TAG, "Media Service sending time message "+message);
                        try{
                        Socket sender = new Socket("127.0.0.1", 12002);
                        ObjectOutputStream objOut = new ObjectOutputStream(sender.getOutputStream());
                        objOut.write(message.getBytes());
                        objOut.flush();
                        sender.close();
                        } catch (IOException e) {
                        Log.e(getClass().getName(), e.getMessage());
                        }
                        handler.postDelayed(tm, 1200);
                        }

	}

	private TimecodeRunnable tm;

	public class NotificationRunnable implements Runnable {
        private String message = null;

		public NotificationRunnable(String m){
                message = m;
                run();
                }

		public void run() {
                if (message != null && message.length() > 0) {
                showNotification(message);
                }
                }

		/**
                * @param message the message to set
                */
                public void setMessage(String message) {
                this.message = message;
                }
                }

	public void showNotification(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
        }

	private void connect(String message){

		try{
                Socket lclSender = new Socket("127.0.0.1", 12001);
                ObjectOutputStream objOut = new ObjectOutputStream(lclSender.getOutputStream());
                objOut.write(message.getBytes());
                objOut.flush();
                lclSender.close();
                } catch (IOException e) {
                Log.e(getClass().getName(), e.getMessage());
                }

	}

	public class LocalBinder extends Binder {
        LinuxmceAudioService getService() {
        // Return this instance of LocalService so clients can call public methods
        return LinuxmceAudioService.this;
        }
        }

	@Override
        public IBinder onBind(Intent arg0) {
        //
        return mBinder;
        }

	public void onCreate(){
        // Create a new media player and set the listeners
        mp=null;
        tm=null;
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);
        Log.d(TAG, "Created Linuxmce Media Service");
        showNotification("Starting Up Media Service");
        }

	public void onDestroy(){
        mp.release();
        }

	@Override 
        public int onStartCommand(Intent intent , int flags, int startId){
        Log.d(TAG, "Started Linuxmce Media Service");
        return Service.START_NOT_STICKY;
        }

	public void setIntentRedeliver(boolean enabled){

	}

	public void setVolume(float vol){
        if(mp!=null){
        Log.d(TAG, "Media Service volume tigger");
        AudioManager audioManager = (AudioManager)getSystemService(AUDIO_SERVICE);
        if(vol < 0 ){
        audioManager.adjustVolume(0xffffffff, 0);
        } else {
        audioManager.adjustVolume(0x00000001, 0);
        }
        float actualVolume = (float)audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        }
        }


	public void playAudio(String url) {
        try {
        //      final String path = mPath.getText().toString();
        final String path = url; //"http://192.168.80.1/lmce-admin/qOrbiterGenerator.php?id=7802";

			Log.d(TAG, "Playing::" + path);
                        if(mp==null){
                        mp= new MediaPlayer();
                        mp.reset();
                        }

			mp.setOnErrorListener(this);
                        mp.setOnBufferingUpdateListener(this);
                        mp.setOnCompletionListener(this);
                        mp.setOnPreparedListener(this);
                        mp.setAudioStreamType(AudioManager.STREAM_MUSIC);

			if(mp.isPlaying()){
                        mp.stop();
                        mp.reset();
                        }

			current = path.toString();

			// Set the data source in another thread
                        // which actually downloads the mp3 or videos
                        // to a temporary location
                        Log.v(TAG, "Preparing: " + path);

			Runnable r = new Runnable() {
                        public void run() {
                        try {
                        mp.setDataSource(current);
                        mp.prepareAsync();
                        } catch (IOException e) {
                        Log.e(TAG, e.getMessage(), e);
                        mp.reset();
                        mp.release();
                        mp=null;


					}
                                        catch (IllegalArgumentException a) {

					}
                                        catch(IllegalStateException s){
                                        mp.reset();
                                        mp.release();
                                        mp=null;
                                        }
                                        catch (NullPointerException n){
                                        mp.reset();
                                        mp.release();
                                        mp=null;


					}
                                        }
                                        };
                                        new Thread(r).start();
                                        } catch (Exception e) {
                                        Log.e(TAG, "error: " + e.getMessage(), e);
                                        if (mp != null) {
                                        mp.release();
                                        mp=null;


			}
                        }
                        }

	public boolean pause(){

if(mp==null){
return false;
}

                if(mp.isPlaying()){
                mp.pause();
                return true;
                } else if(!mp.isPlaying()){
                mp.start();
                return true;
                }
                return false;

	}

	public boolean resume(){
        if(mp!=null || !mp.isPlaying()){
        mp.start();
        return true;
        } else if(mp.isPlaying()){
        mp.pause();
        return true;
        }
        return false;
        }

	public boolean seek(int msec){
        if(mp!=null && mp.isPlaying()){
        if(msec < mp.getDuration()){
        mp.seekTo(msec);
        return true;
        }
        }
        return false;
        }
        public boolean stop(){
        if(mp!=null){
        Log.d(TAG, "Audio Player Stop Called");
        if(mp.isPlaying()){
        mp.stop();
        mp.release();
        mp=null;
        }
        else{
        mp.release();
        mp=null;

			}
                        }
                        return true;
                        }

	public int getElapsed(){

        if(mp!=null ){
        return mp.getCurrentPosition();
        }else{
        return 0;
        }
        }


	/**
        * @param args
        */
        public static void main(String[] args) {
        // TODO Auto-generated method stub

	}


	public boolean onError(MediaPlayer mediaPlayer, int what, int extra) {
        Log.e(TAG, "onError--->   what:" + what + "    extra:" + extra);
        if(mp!=null){
        mp.reset();
        mp.release();
        mp=null;
        }

        connect("\n{event:completed}\n");
        connect("\n{error:mediaplayer error.}\n");
        return true;

	}

	public void onBufferingUpdate(MediaPlayer arg0, int percent) {
        //Log.d(TAG, "onBufferingUpdate called --->   percent:" + percent);
        }



	public void onCompletion(MediaPlayer arg0) {
        Log.d(TAG, "onCompletion called");
        mp.release();
        mp=null;
        connect("\n{event:completed}\n");
        }

	public void onPrepared(MediaPlayer mediaplayer) {
        Log.d(TAG, "onPrepared called");
        mp.start();
        connect("\n{time:"+mp.getDuration()+"}\n");
        if(tm==null){
        tm = new TimecodeRunnable();
        }

                //handler.postDelayed(tm, 1000);
                Log.v(TAG, "Duration:  ===>" + mp.getDuration());

	}


}
