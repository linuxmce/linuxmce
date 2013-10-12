/**
 * 
 */
package org.kde.necessitas.origo;

import android.net.Uri;
import android.os.Binder;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.media.MediaPlayer;
import android.util.Log;
import java.io.IOException;
import org.kde.necessitas.origo.MediaCallbackInterface;

import android.media.AudioManager;
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
	MediaCallbackInterface mediaBridge;



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
		Log.d(TAG, "Created Linuxmce Audio Service");	
		mediaBridge = new MediaCallbackInterface();

		if(mediaBridge!= null){
			
		} else{
			Log.d(TAG,"Couldnt initialize Audio Service bridge!");
		}
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
			final String path = url; //"http://192.168.80.1/lmce-admin/qOrbiterGenerator.php?id=7802";
			Log.d(TAG, "Playing::" + path);
			

			if(mp==null){
				mp= new MediaPlayer();
								
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
					}
				}
			};
			new Thread(r).start();
		} catch (Exception e) {
			Log.e(TAG, "error: " + e.getMessage(), e);
			if (mp != null) {
				mp.stop();
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
				mediaBridge.setMediaPlaying(false);
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
		if (mp != null) {			

		}
		return true;
	}

	public void onBufferingUpdate(MediaPlayer arg0, int percent) {
		//	Log.d(TAG, "onBufferingUpdate called --->   percent:" + percent);
	}

	public void onCompletion(MediaPlayer arg0) {
		Log.d(TAG, "onCompletion called");
		mp.release();
		mp=null;
		mediaBridge.setMediaPlaying(false);
	}

	public void onPrepared(MediaPlayer mediaplayer) {
		Log.d(TAG, "onPrepared called");
		mp.start();
		mediaBridge.setMediaPlaying(true);
		mediaBridge.setAndroidTotalTime(mp.getDuration());
		Log.v(TAG, "Duration:  ===>" + mp.getDuration());

	}



}
