

#import "ConfigViewController.h"


@implementation ConfigViewController
@synthesize scrollView, pollIntervalLabel, pollIntervalSlider;

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewWillAppear:(BOOL)animated {
	NSLog(@"Configview starting up...");
	scrollView.contentSize = CGSizeMake([[UIScreen mainScreen] bounds].size.width,scrollView.frame.size.height+self.tabBarController.tabBar.frame.size.height);
	scrollView.autoresizingMask = UIViewAutoresizingFlexibleHeight;
	scrollView.maximumZoomScale = 4.0;
	scrollView.minimumZoomScale = 0.75;
	//scrollView.clipsToBounds = YES;
	scrollView.delegate = self;

	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	[wifi  setOn:[prefs boolForKey:@"connection_wifi"]];
	[loadIndicator setOn:[prefs boolForKey:@"connection_loadindicator"]];

	 //[prefs integerForKey:@"delay_preference"];

	[super viewDidLoad];
}

-(IBAction) sliderChanged:(id) sender {
	int progressAsInt = (int)(pollIntervalSlider.value + 0.5f);
	self.pollIntervalLabel.text = [[NSString alloc]
								   initWithFormat:@"every %d seconds",progressAsInt];
}

-(IBAction) savePreferences:(id) sender {
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	[prefs setBool:[wifi isOn] forKey:@"connection_wifi"];
	[prefs setBool:[loadIndicator isOn] forKey:@"connection_loadindicator"];

	[prefs synchronize];
	
	[navigator popViewControllerAnimated:YES];
}

- (IBAction)onExit: (id)sender {
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
	[super didReceiveMemoryWarning];

	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[super dealloc];
	[scrollView release];
}

@end
