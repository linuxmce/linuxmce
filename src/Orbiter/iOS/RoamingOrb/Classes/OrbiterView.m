//
//  Created by Serge Wagener (Foxi352) on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//
//  29/09/10 - Fixed memory leak (Foxi352)
//  09/10/10 - Fixed reconnecting after connection loss (Foxi352)

#import "OrbiterView.h"
#import "proxy-orbiter-client.h"
#import "reachability.h"

#define degreesToRadian(x) (M_PI * (x) / 180.0)

@implementation OrbiterView

NSInteger pollInterval = 2;
NSInteger connectInterval = 5;

BOOL connected = NO, isWifi = NO, needsWifi = YES, imageloadindicator = YES;

NSString *deviceType;
proxy_orbiter_client *orbiter;
UIImageView *orbiterImageView;
UIActivityIndicatorView *activityIndicator, *pollingIndicator;
UILabel *connectingLabel;

- (void)loadView {
	
	UIView *view = [[UIView alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];
	self.view = view;

	CGSize screenSize = [[UIScreen mainScreen] bounds].size;
	self.view.transform = CGAffineTransformMakeRotation(degreesToRadian(90));
    self.view.bounds = CGRectMake(0.0f, 0.0f, screenSize.height, screenSize.width);
	self.view.center = CGPointMake(screenSize.width/2, screenSize.height/2);
	//[view release];
}

-(void)viewDidLoad {
	
	deviceType = [UIDevice currentDevice].name;	
	NSLog(@"Starting up on %@...", deviceType);
	
	if(self.isIPad) NSLog(@"Running on an iPad");
	else NSLog(@"Not running on an iPad");
	
	CGSize screenSize = [[UIScreen mainScreen] bounds].size;
	NSLog(@"Screensize %3.0fx%3.0f, screenscale %1.0f",screenSize.height, screenSize.width, self.mainScreenScale);
	
	if(self.isIPad) orbiterImageView = [[UIImageView alloc] initWithImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"splash-ipad" ofType:@"png"]]];
	else orbiterImageView = [[UIImageView alloc] initWithImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"splash" ofType:@"png"]]];
	orbiterImageView.contentMode = UIViewContentModeScaleAspectFit;
    orbiterImageView.frame = CGRectMake(0, 0, screenSize.height, screenSize.width);
	[self.view addSubview:orbiterImageView];
	[orbiterImageView release];
	
	activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
	if(self.isIPad) activityIndicator.center = CGPointMake(330, 350);
	else activityIndicator.center = CGPointMake(160, 140);
	activityIndicator.hidesWhenStopped = YES;
	[activityIndicator startAnimating];
	[self.view addSubview:activityIndicator];
	[activityIndicator release];
	
	pollingIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
	pollingIndicator.center = CGPointMake(screenSize.height/2, screenSize.width/2);
	pollingIndicator.hidesWhenStopped = YES;
	[pollingIndicator stopAnimating];
	[self.view addSubview:pollingIndicator];
	[pollingIndicator release];

	if(self.isIPad) connectingLabel = [[UILabel alloc] initWithFrame:CGRectMake(5, 580, 240, 20)];
	else connectingLabel = [[UILabel alloc] initWithFrame:CGRectMake(5, 290, 240, 20)];
	connectingLabel.text = @"Connecting to proxy on core ...";
	connectingLabel.backgroundColor = [UIColor clearColor];
	[connectingLabel setHidden:NO];
	[self.view addSubview:connectingLabel];
	[connectingLabel release];
			
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	
	isWifi = ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] == ReachableViaWiFi);
	needsWifi = [[NSUserDefaults standardUserDefaults] boolForKey:@"wifionly_preference"]; 
	imageloadindicator = [[NSUserDefaults standardUserDefaults] boolForKey:@"imageloadindicator_preference"]; 
	
	if(needsWifi && !isWifi) {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Connection error" message:@"According to your preferences you must be connected to WiFi ! Please activate Wifi or change settings !" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
	}
	else {
		orbiter = [[proxy_orbiter_client alloc]init:self];	
		connectTimer = [[NSTimer 
		scheduledTimerWithTimeInterval:(connectInterval)
		target:self
		selector:@selector(connectToServer:)
		userInfo:nil
		repeats:YES] retain];
		[connectTimer fire];	
	}
	[super viewDidLoad];
}

- (void)viewWillAppear:(BOOL)animated {	
}

- (void)connectToServer:(id)sender {
	// open connection to server. Rest will be handled by delegates

	NSString* serverip = [[NSUserDefaults standardUserDefaults] objectForKey:@"serverip_preference"]; 
	NSInteger serverport = [[NSUserDefaults standardUserDefaults] integerForKey:@"serverport_preference"]; 
	pollInterval = [[NSUserDefaults standardUserDefaults] integerForKey:@"delay_preference"]; 
	if(pollInterval < 1) pollInterval = 1;
	[orbiter connect:serverip port:serverport]; 	
}

- (void)pollServer:(id)sender {
    [orbiter poll];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	if(imageloadindicator) [pollingIndicator startAnimating];	
	UITouch *touch = [touches anyObject];
	CGPoint touchCoordinates = [touch locationInView:self.view];
	//NSLog(@"Touch x:%3.0f y:%3.0f",touchCoordinates.x, touchCoordinates.y);
	[orbiter sendTouch:(NSInteger)touchCoordinates.x*self.mainScreenScale y:(NSInteger)touchCoordinates.y*self.mainScreenScale];
	//[orbiter downloadImage];
}

- (void)didConnect {
	[connectTimer invalidate];
	[connectTimer release];
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
	[orbiterImageView setImage:orbiterImage];
	[orbiterImage release];
/*
	pollTimer = [[NSTimer 
				  scheduledTimerWithTimeInterval:(pollInterval)
				  target:self
				  selector:@selector(pollServer:)
				  userInfo:nil
				  repeats:YES] retain];*/
	if(imageloadindicator) [pollingIndicator stopAnimating];	
}

- (void)didReceiveConnectionError:(NSString *)errorMessage {
	if(connected) {
		[pollTimer invalidate];
		[pollTimer release];
		connected = NO;
		if(self.isIPad) [orbiterImageView setImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"splash-ipad" ofType:@"png"]]];
		else [orbiterImageView setImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"splash" ofType:@"png"]]];
		[activityIndicator startAnimating];
		[connectingLabel setHidden:NO];
		connectTimer = [[NSTimer 
					 scheduledTimerWithTimeInterval:(connectInterval)
					 target:self
					 selector:@selector(connectToServer:)
					 userInfo:nil
					 repeats:YES] retain];
	}
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Memory warning" message:@"Oh oh. This should not happen. Please report to linuxmce team !" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];	
    [super didReceiveMemoryWarning];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return NO;
    NSLog(@"Device rotated !");
	if(interfaceOrientation == UIInterfaceOrientationLandscapeRight)
        return YES; 
	if(interfaceOrientation == UIInterfaceOrientationLandscapeLeft)
		return YES;
	return NO;
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

- (CGFloat) mainScreenScale {
	CGFloat scale = 1.0;
	if ([UIScreen instancesRespondToSelector:@selector(scale)]) {
		scale = [[UIScreen mainScreen] scale];
	}
	return scale;
}

- (BOOL) isIPad {
	BOOL isIPad=NO;
	NSString* model = [UIDevice currentDevice].model;
	if ([model rangeOfString:@"iPad"].location != NSNotFound) {
		return YES;
	}
	return isIPad;
}

- (void)dealloc {
    [super dealloc];
}

@end
