/**
 * 
 */
package org.kde.necessitas.origo;


import android.os.Binder;
import android.os.StrictMode;
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
import android.os.Handler;


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
	private static final String TAG = "LinuxMCE Audio Service";
	private final Handler handler = new Handler();
	private MediaPlayer mp;
	private String current;


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
			Socket sender = new Socket("127.0.0.1", 12001);

			ObjectOutputStream objOut = new ObjectOutputStream(sender.getOutputStream());
			objOut.write(message.getBytes());
			objOut.flush(); 			
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
                StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
                StrictMode.setThreadPolicy(policy);
		Log.d(TAG, "Created Linuxmce Audio Service");	
		showNotification("Starting Up Audio Service");
	}

	public void onDestroy(){
		mp.release();
	}

	@Override 
	public int onStartCommand(Intent intent , int flags, int startId){
		Log.d(TAG, "Started Linuxmce Audio Service");

		return Service.START_NOT_STICKY;
	}

	public void setIntentRedeliver(boolean enabled){

	}
	
	public void setVolume(float vol){
		if(mp!=null){
		mp.setVolume(vol, vol);
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

	public void stop(){
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
	}





/**
 * @param args
 */
public static void main(String[] args) {
	// TODO Auto-generated method stub

}


public boolean onError(MediaPlayer mediaPlayer, int what, int extra) {
	Log.e(TAG, "onError--->   what:" + what + "    extra:" + extra);
	mp.reset();
	mp.release();
	mp=null;
	connect("\n{event:completed}\n");
	connect("\n{error:mediaplayer error.}\n");
	return true;

}

public void onBufferingUpdate(MediaPlayer arg0, int percent) {
	//	Log.d(TAG, "onBufferingUpdate called --->   percent:" + percent);
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
	Log.v(TAG, "Duration:  ===>" + mp.getDuration());

}
}
