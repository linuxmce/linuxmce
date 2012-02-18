

#import "OrbViewController.h"
#import "RoamingOrbAppDelegate.h"

@implementation OrbViewController

@synthesize orbView;
@synthesize swipeUpRecognizer, swipeDownRecognizer, swipeLeftRecognizer, swipeRightRecognizer, tabBar;

proxy_orbiter_client *orbiter;
CGSize screenSize;
UIView *previousTabBarView;
NSString *deviceType;

BOOL connected = NO, isWifi = NO, needsWifi = YES, ssl = NO, imageLoadIndicator = YES, isTabbarVisible = YES;
BOOL executePoll = NO, incompleteSettings = NO;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Show splash until first image loaded
    if(self.isIPad) [orbView setImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"splash-ipad" ofType:@"png"]]];
    else {
        UIImage *myImage=[UIImage imageNamed:@"splash.png"];
        [orbView setImage:myImage];
    }

    orbiter = [[proxy_orbiter_client alloc] init:self];

	// find out on which device and at which resolution we run
    deviceType = [UIDevice currentDevice].name;
	NSLog(@"Starting up on %@...", deviceType);
	if(self.isIPad) NSLog(@"Running on an iPad");
	else NSLog(@"Not running on an iPad");
	screenSize = [[UIScreen mainScreen] bounds].size;
	NSLog(@"Screensize %3.0fx%3.0f, screenscale %1.0f",screenSize.height, screenSize.width, self.mainScreenScale);

	//register up swipe recognizer to show tabbar
	UIGestureRecognizer *recognizer;
	recognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
	self.swipeUpRecognizer = (UISwipeGestureRecognizer *)recognizer;
	swipeUpRecognizer.direction = UISwipeGestureRecognizerDirectionUp;
	[self.view addGestureRecognizer:swipeUpRecognizer];
	[recognizer release];

	//register down swipe recognizer to hide tabbar
	recognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
	self.swipeDownRecognizer = (UISwipeGestureRecognizer *)recognizer;
	swipeDownRecognizer.direction = UISwipeGestureRecognizerDirectionDown;
	[self.view addGestureRecognizer:swipeDownRecognizer];
	[recognizer release];

	//resize view to fullscreen to avoid white bar when tabbar is hidden
	UIView *transView = [self.tabBarController.view.subviews objectAtIndex:0];
    transView.frame = CGRectMake(0, 0, screenSize.width, screenSize.height);
    
	// resize UIImageView to scale full screen (iPhone, iPad, ...)
	orbView.contentMode = UIViewContentModeScaleAspectFit;
	orbView.frame = CGRectMake(0, 0, screenSize.height, screenSize.width);
}

- (void)viewWillAppear:(BOOL)animated {
	// reading preferences, setting defaults if none set
    incompleteSettings = NO;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	if (![prefs valueForKey:@"server"]) {
        NSLog(@"Server is not defined");
        [prefs setObject:@"192.168.80.1" forKey:@"server"];
        incompleteSettings = YES;
    }
	if (![prefs valueForKey:@"port"]) {
        NSLog(@"Port is not defined"); 
        incompleteSettings = YES;
    }
	if (![prefs integerForKey:@"connectionInterval"]) [prefs setInteger:5 forKey:@"connectionInterval"];
	if (![prefs integerForKey:@"pollInterval"]) [prefs setInteger:3 forKey:@"pollInterval"];
    if (![prefs valueForKey:@"wifi"]) [prefs setBool:YES forKey:@"wifi"];   
	if (![prefs valueForKey:@"loadindicator"]) [prefs setBool:YES forKey:@"loadindicator"];
   	[prefs synchronize];
    
    [orbiter setServer:[prefs objectForKey:@"server"]];
	[orbiter setPort:[prefs integerForKey:@"port"]];
    
    needsWifi = [prefs boolForKey:@"wifi"];
	imageLoadIndicator = [prefs boolForKey:@"loadindicator"];
	connectInterval = [prefs integerForKey:@"connectionInterval"];
    pollInterval = [prefs integerForKey:@"pollInterval"];
    
	// fire "poll ANYNEWS" timer if network requirements are met
	isWifi = ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] == ReachableViaWiFi);
    
    if(incompleteSettings) {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Connection error" message:@"Your settings are not completed. You have been redirected to settings screen prefilled with some default settings." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
        [self showTabBar:YES];
        [self.tabBarController setSelectedIndex:1];
    }
    else if(needsWifi && !isWifi) {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Connection error" message:@"According to your preferences you must be connected to WiFi ! Please activate Wifi or change settings !" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
        [self showTabBar:YES];
        [self.tabBarController setSelectedIndex:1];
	}
	else {        
		// fire connection timer
        [self showTabBar:NO];
        [self startConnecting];
	}
}

- (void)viewWillDisappear:(BOOL)animated {
	[self stopPolling];
    [self stopConnecting];
    
	[super viewWillDisappear:animated];
}

- (void)showTabBar:(BOOL)visible {
    if(isTabbarVisible && !visible) {
        NSLog(@"Tabbar is hiding ...");
        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDuration:0.55];
        
        CGRect tabBarFrame = self.tabBarController.tabBar.frame;
        [tabBar setAlpha:0.0];
        isTabbarVisible=FALSE;
        tabBarFrame.origin.y = tabBarFrame.origin.y + tabBarFrame.size.height;
        self.tabBarController.tabBar.frame = tabBarFrame;
        [UIView commitAnimations];
    }
    else if(!isTabbarVisible && visible) {
        NSLog(@"Tabbar is showing up ...");
        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDuration:0.55];
        
        CGRect tabBarFrame = self.tabBarController.tabBar.frame;
        [tabBar setAlpha:1.0];		// fade in tabbar
        isTabbarVisible=TRUE;
        tabBarFrame.origin.y = tabBarFrame.origin.y - tabBarFrame.size.height;
        self.tabBarController.tabBar.frame = tabBarFrame;
        [UIView commitAnimations];
    }
}

- (void)connectToServer:(id)sender {
	// open connection to server. Rest will be handled by delegates
    [orbiter connect]; 	
}

- (void)startPolling {
	NSLog(@"Starting polling...");
    // Show a waiting HUD
    /*if(imageLoadIndicator) {
        HUD = [[MBProgressHUD alloc] initWithView:self.view];
        [self.view addSubview:HUD];
        HUD.delegate = self;
        HUD.labelText = @"Loading from core...";
        [HUD show:YES];
    }*/
	executePoll = YES;
    // setup a timer that polls proxy-orb every x seconds
    pollTimer = [[NSTimer 
                  scheduledTimerWithTimeInterval:(pollInterval)
                  target:self
                  selector:@selector(pollServer:)
                  userInfo:nil
                  repeats:YES] retain];
    //[pollTimer fire];
}

- (void)stopPolling {
	NSLog(@"Stopping polling...");
	executePoll = NO;
	[pollTimer invalidate];
	pollTimer = nil;
}

- (void)startConnecting {
	NSLog(@"Starting connection tries...");
    // Show a waiting HUD
    HUD = [[MBProgressHUD alloc] initWithView:self.view];
    [self.view addSubview:HUD];
    HUD.delegate = self;
    HUD.labelText = @"Connecting to core";
    [HUD show:YES];
    connected = NO;
	executePoll = NO;
    connectTimer = [[NSTimer 
                     scheduledTimerWithTimeInterval:(connectInterval)
                     target:self
                     selector:@selector(connectToServer:)
                     userInfo:nil
                     repeats:YES] retain];
    [connectTimer fire];
}

- (void)stopConnecting {
    //if(HUD != nil) [HUD hide:YES];
	NSLog(@"Stopping connection tries...");
	[connectTimer invalidate];
	connectTimer = nil;
}

- (void)didConnect {
	[self stopConnecting];
	if(!connected) {
		connected = YES;
		NSLog(@"Connected, starting polling...");
        HUD.labelText = @"Loading from core...";
        [self startPolling];
	}
}

- (void)didReceiveConnectionError:(NSString *)errorMessage {
	NSLog(@"didReceiveConnectionError fired: %@", errorMessage);
    if(connected) {
        [self stopPolling];
		connected = NO;
        if(self.isIPad) [orbView setImage:[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"splash-ipad" ofType:@"png"]]];
        else {
            UIImage *myImage=[UIImage imageNamed:@"splash.png"];
            [orbView setImage:myImage];
        }
        if(HUD != nil) [HUD hide:YES];
        [self startConnecting];
	}
    else {
        if(HUD != nil) [HUD hide:YES];
    }

}

- (void)pollServer:(id)sender {
	if(executePoll) {
		[orbiter poll];
	} else {
		NSLog(@"Polling semaphore locked...");
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    if(imageLoadIndicator) {
        // Show a waiting HUD
        HUD = [[MBProgressHUD alloc] initWithView:self.view];
        [self.view addSubview:HUD];
        HUD.delegate = self;
        //HUD.labelText = @"Loading ...";
        [HUD show:YES];
    }
	UITouch *touch = [touches anyObject];
	CGPoint touchCoordinates = [touch locationInView:self.view];
	[orbiter sendTouch:(NSInteger)touchCoordinates.x*self.mainScreenScale y:(NSInteger)touchCoordinates.y*self.mainScreenScale];
}

- (void)handleSwipeFrom:(UISwipeGestureRecognizer *)recognizer {
	[UIView beginAnimations:nil context:NULL];
	[UIView setAnimationDuration:0.55];

	CGRect tabBarFrame = self.tabBarController.tabBar.frame;
	int tabBarHeight = tabBarFrame.size.height;
	int offset = 0;

	if (recognizer.direction == UISwipeGestureRecognizerDirectionUp) {
		NSLog(@"Swipe up, show tabbar ...");
		if(!isTabbarVisible) {
			[tabBar setAlpha:1.0];		// fade in tabbar
			offset = -1*tabBarHeight;	// move tabbar up
			isTabbarVisible=TRUE;
		}
	}
	else {
		if(isTabbarVisible) {
			NSLog(@"Swipe down, hide tabbar ...");
			[tabBar setAlpha:0.0];		//fade out tabbar
			offset = tabBarHeight;		// move tabbar down
			isTabbarVisible=FALSE;
		}
	}

	// move tabbar
	int tabBarY = tabBarFrame.origin.y + offset;
	tabBarFrame.origin.y = tabBarY;
	self.tabBarController.tabBar.frame = tabBarFrame;

	[UIView commitAnimations];
}

/*
- (void)didReceiveNews {
	NSLog(@"didReceiveNews fired");
    if(imageLoadIndicator) {
        [activityIndicator startAnimating];
        // Show a waiting HUD
        HUD = [[MBProgressHUD alloc] initWithView:self.view];
        [self.view addSubview:HUD];
        HUD.delegate = self;
        HUD.labelText = @"Loading from core...";
        [HUD show:YES];    
    }
	[orbiter downloadImage];
}*/

- (void)didReceiveNewImage:(UIImage*)orbiterImage {
	NSLog(@"didReceiveNewImage fired %3.0fx%3.0f", orbiterImage.size.width, orbiterImage.size.height);
	[orbView setImage:orbiterImage];
	[orbiterImage release];
    if(HUD != nil) [HUD hide:YES];
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

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations
	if(interfaceOrientation == UIInterfaceOrientationLandscapeRight)
		return YES;
	if(interfaceOrientation == UIInterfaceOrientationLandscapeLeft)
		return YES;
	return NO;
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Memory warning" message:@"Oh oh. This should not happen. Please report to linuxmce team !" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];	
	[super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
	[super viewDidUnload];

	self.swipeUpRecognizer = nil;
	self.swipeDownRecognizer = nil;

}

- (void)dealloc {
	[super dealloc];
}

#pragma mark MBProgressHUDDelegate methods

- (void)hudWasHidden:(MBProgressHUD *)hud {
    // Remove HUD from screen when the HUD was hidded
    [HUD removeFromSuperview];
    [HUD release];
    HUD = nil;
}

@end
