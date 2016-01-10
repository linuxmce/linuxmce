package org.kde.necessitas.origo;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.*;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.LinearLayout;

import java.io.IOException;

public class VideoActivity extends Activity implements OnErrorListener,
  OnBufferingUpdateListener, OnCompletionListener,
  MediaPlayer.OnPreparedListener, SurfaceHolder.Callback {

  private static final String TAG = "VideoPlayer";

  private MediaPlayer mp;
  private SurfaceView mPreview;
  private SurfaceHolder holder;
  private String current;

    /**
    * Called when the activity is first created.
    */
    public void onCreate(Bundle icicle) {
      super.onCreate(icicle);
      requestWindowFeature(Window.FEATURE_NO_TITLE);

      getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN |
          WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON |
          WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
      getWindow().setFormat(PixelFormat.TRANSPARENT);

      LinearLayout layout = new LinearLayout(getBaseContext());
      mPreview = new SurfaceView(layout.getContext());
      layout.addView(mPreview);
      setContentView(layout);

      // Set a size for the video screen
      holder = mPreview.getHolder();
      holder.addCallback(this);
      holder.setKeepScreenOn(true);
    }


    private void playVideo() {
    try {
      //      final String path = mPath.getText().toString();
      final String path = "http://192.168.80.1/lmce-admin/qOrbiterGenerator.php?id=20818";

      Log.v(TAG, "path: " + path);

      // If the path has not changed, just start the media player
      if (path.equals(current) && mp != null) {
        mp.start();
        return;
      }
      current = path;

      // Create a new media player and set the listeners
      mp = new MediaPlayer();
      mp.setOnErrorListener(this);
      mp.setOnBufferingUpdateListener(this);
      mp.setOnCompletionListener(this);
      mp.setOnPreparedListener(this);
      mp.setAudioStreamType(2);

      // Set the surface for the video output
      mp.setDisplay(holder);

      // Set the data source in another thread
      // which actually downloads the mp3 or videos
      // to a temporary location
      Runnable r = new Runnable() {
        public void run() {
          try {
            mp.setDataSource(path);
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

public boolean onError(MediaPlayer mediaPlayer, int what, int extra) {
Log.e(TAG, "onError--->   what:" + what + "    extra:" + extra);
if (mediaPlayer != null) {
mediaPlayer.stop();
mediaPlayer.release();
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

public void surfaceCreated(SurfaceHolder surfaceholder) {
Log.d(TAG, "surfaceCreated called");
playVideo();
}

public void surfaceChanged(SurfaceHolder surfaceholder, int i, int j, int k) {
Log.d(TAG, "surfaceChanged called");
}

public void surfaceDestroyed(SurfaceHolder surfaceholder) {
Log.d(TAG, "surfaceDestroyed called");
}
}
