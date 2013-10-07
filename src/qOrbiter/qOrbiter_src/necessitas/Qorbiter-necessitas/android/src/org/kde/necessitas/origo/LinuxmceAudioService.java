/**
 * 
 */
package org.kde.necessitas.origo;

import android.os.Binder;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.media.MediaPlayer;
import android.util.Log;
import java.io.IOException;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnPreparedListener;
/**
 * @author langston
 *
 */
public class LinuxmceAudioService extends Service implements
OnBufferingUpdateListener, OnErrorListener, OnCompletionListener, 
OnPreparedListener {

	private final IBinder mBinder = new LocalBinder();
	private static final String TAG = "LinuxMCE Audio Service";
	private MediaPlayer mp;
	private String current;



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
		mp = new MediaPlayer();
		mp.setOnErrorListener(this);
		mp.setOnBufferingUpdateListener(this);
		mp.setOnCompletionListener(this);
		mp.setOnPreparedListener(this);
		mp.setAudioStreamType(2);
		Log.d(TAG, "Created Linuxmce Audio Service");	

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


	public void playAudio(String url) {
		try {
			//      final String path = mPath.getText().toString();
			final String path = url; //"http://192.168.80.1/lmce-admin/qOrbiterGenerator.php?id=20818";

			Log.v(TAG, "path: " + path);

			
			current = path;

			// Set the data source in another thread
			// which actually downloads the mp3 or videos
			// to a temporary location
			Runnable r = new Runnable() {
				public void run() {
					try {
						mp.setDataSource(current);
						Log.v(TAG, "Preparing: " + path);
						mp.prepare();
					} catch (IOException e) {
						Log.e(TAG, e.getMessage(), e);
					}
					Log.v(TAG, "Duration:  ===>" + mp.getDuration());
					mp.start();
				}
			};
			new Thread(r).start();
		} catch (Exception e) {
			Log.e(TAG, "error: " + e.getMessage(), e);
			if (mp != null) {
				mp.stop();
				mp.release();
			}
		}
	}

	public void stop(){
		mp.stop();
		mp.reset();
	}



	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}




	public boolean onError(MediaPlayer mediaPlayer, int what, int extra) {
		Log.e(TAG, "onError--->   what:" + what + "    extra:" + extra);
		if (mp != null) {
			mp.stop();
			mp.release();
		}
		return true;
	}

	public void onBufferingUpdate(MediaPlayer arg0, int percent) {
		Log.d(TAG, "onBufferingUpdate called --->   percent:" + percent);
	}

	public void onCompletion(MediaPlayer arg0) {
		Log.d(TAG, "onCompletion called");
		
	}

	public void onPrepared(MediaPlayer mediaplayer) {
		Log.d(TAG, "onPrepared called");
	}



}
