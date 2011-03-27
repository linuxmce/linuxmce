//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioToolbox.h>

@interface OrbiterView : UIViewController <UIAccelerometerDelegate>{
	NSTimer *pollTimer, *connectTimer;
	UIImageView *orbiterImageView;
	
}
 
- (void)didReceiveNewImage:(UIImage*)orbiterImage;
- (void)didReceiveConnectionError:(NSString *)errorMessage;
- (void)didConnect;
- (void)connectToServer:(id)sender ;
- (void)vibrate;
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (CGFloat) mainScreenScale;
- (BOOL) isIPad;

@end
