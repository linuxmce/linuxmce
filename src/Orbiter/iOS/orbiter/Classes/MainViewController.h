//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioToolbox.h>

@interface MainViewController : UIViewController <UIAccelerometerDelegate>{
	IBOutlet UIImageView *orbiterView;
	IBOutlet UILabel *connectingLabel;
	IBOutlet UIActivityIndicatorView *activityIndicator;
	NSTimer *pollTimer, *connectTimer;
}
 
@property (nonatomic, retain) UIImageView *orbiterView;

- (void)didReceiveNewImage:(UIImage*)orbiterImage;
- (void)didReceiveConnectionError:(NSString *)errorMessage;
- (void)didConnect;
- (void)connectToServer:(id)sender ;
- (void)vibrate;
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;

@end
