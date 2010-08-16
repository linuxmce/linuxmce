//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import "MainViewController.h"
#import "proxy-orbiter-client.h"
#import "reachability.h"

@implementation MainViewController
@synthesize orbiterView;

NSInteger pollInterval = 2;
const int connectInterval = 5;

BOOL connected = NO, isWifi = NO, needsWifi = YES;

NSString *deviceType;
proxy_orbiter_client *orbiter;

-(void)viewDidLoad {
	[super viewDidLoad];
	[orbiterView setImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"back" ofType:@"png"]]];
	[activityIndicator startAnimating];
	[connectingLabel setHidden:NO];
	deviceType = [UIDevice currentDevice].name;	
	NSLog(@"Starting up on %@...", deviceType);
	
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	connectTimer = [[NSTimer 
					 scheduledTimerWithTimeInterval:(connectInterval)
					 target:self
					 selector:@selector(connectToServer:)
					 userInfo:nil
					 repeats:YES] retain];
	[connectTimer fire];
	isWifi = ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] == ReachableViaWiFi);
	needsWifi = [[NSUserDefaults standardUserDefaults] boolForKey:@"wifionly_preference"]; 
	
	if(needsWifi && !isWifi) {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Connection error" message:@"According to your preferences you must be connected to WiFi ! Please activate Wifi or change settings !" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
	}
}

- (void)connectToServer:(id)sender {
	// open connection to server. Rest will be handled by delegates
	orbiter = [[proxy_orbiter_client alloc]init:self];	

	NSString* serverip = [[NSUserDefaults standardUserDefaults] objectForKey:@"serverip_preference"]; 
	NSInteger serverport = [[NSUserDefaults standardUserDefaults] integerForKey:@"serverport_preference"]; 
	pollInterval = [[NSUserDefaults standardUserDefaults] integerForKey:@"delay_preference"]; 
	isWifi = ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] == ReachableViaWiFi);
	if(!isWifi && needsWifi) {
	}
	[orbiter connect:serverip port:serverport]; 	
}

- (void)pollServer:(id)sender {
    [orbiter poll];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint touchCoordinates = [touch locationInView:self.view];
	[orbiter sendTouch:(NSInteger)touchCoordinates.x y:(NSInteger)touchCoordinates.y];
	//[orbiter downloadImage];
}

- (void)didConnect {
	[connectTimer invalidate];
	if(!connected) {
		connected = YES;
		NSLog(@"Connected, starting polling...");
		// setup a timer that polls proxy-orb every x seconds
		pollTimer = [[NSTimer 
				  scheduledTimerWithTimeInterval:(pollInterval)
				  target:self
				  selector:@selector(pollServer:)
				  userInfo:nil
				  repeats:YES] retain];
		[activityIndicator stopAnimating];
		[connectingLabel setHidden:YES];

	}
}

- (void)didReceiveNews {
	NSLog(@"didReceiveNews fired");
	[orbiter downloadImage];
}

- (void)didReceiveNewImage:(UIImage*)orbiterImage {
	NSLog(@"didReceiveNewImage fired");
	[orbiterView setImage:orbiterImage];
}

- (void)didReceiveConnectionError:(NSString *)errorMessage {
	[pollTimer invalidate];
	if(connected) {
		connected = NO;
		connectTimer = [[NSTimer 
					 scheduledTimerWithTimeInterval:(connectInterval)
					 target:self
					 selector:@selector(connectToServer:)
					 userInfo:nil
					 repeats:YES] retain];
		[orbiterView setImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"back" ofType:@"png"]]];
		[activityIndicator startAnimating];
		[connectingLabel setHidden:NO];
	}
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    if(interfaceOrientation == UIInterfaceOrientationPortrait)
        return NO;
    if(interfaceOrientation == UIInterfaceOrientationLandscapeRight || interfaceOrientation == UIInterfaceOrientationLandscapeLeft )
        return YES; 
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

// to stress user :-)
- (void)vibrate {
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

// reload IMAGE if iphone is shaked
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration {	
	BOOL shake = [[NSUserDefaults standardUserDefaults] boolForKey:@"shake_preference"]; 
	if(shake) {
		double const kThreshold = 2.0;
		if(fabsf(acceleration.x) > kThreshold || fabsf(acceleration.y) > kThreshold || fabsf(acceleration.z) > kThreshold) {
			NSLog(@"Shake detected, reloading image !");
			[self vibrate];
			[orbiter downloadImage];
		}
	}
}

- (void)dealloc {
    [super dealloc];
}

@end
