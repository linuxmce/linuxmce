

#import "ConfigViewController.h"


@implementation ConfigViewController
@synthesize scrollView, pollIntervalLabel, connectionIntervalLabel;
@synthesize server, port, pollIntervalSlider, connectionIntervalSlider;

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
    [server setText:[prefs objectForKey:@"server"]];
    [port setText:[prefs objectForKey:@"port"]];

    [wifi  setOn:[prefs boolForKey:@"wifi"]];
	[loadIndicator setOn:[prefs boolForKey:@"loadindicator"]];

    [connectionIntervalSlider setValue:[prefs integerForKey:@"connectionInterval"]];
    [pollIntervalSlider setValue:[prefs integerForKey:@"pollInterval"]];
	pollIntervalLabel.text = [[NSString alloc] initWithFormat:@"every %d seconds",[prefs integerForKey:@"pollInterval"]];
	connectionIntervalLabel.text = [[NSString alloc] initWithFormat:@"every %d seconds",[prefs integerForKey:@"connectionInterval"]];
    
	[super viewDidLoad];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return NO;
}

-(IBAction) pollSliderChanged:(id) sender {
    int progressAsInt = (int)(pollIntervalSlider.value + 0.5f);
	self.pollIntervalLabel.text = [[NSString alloc]
								   initWithFormat:@"every %d seconds",progressAsInt];
}

-(IBAction) connectionSliderChanged:(id) sender {
    int progressAsInt = (int)(connectionIntervalSlider.value + 0.5f);
	self.connectionIntervalLabel.text = [[NSString alloc]
								   initWithFormat:@"every %d seconds",progressAsInt];
}

-(IBAction) savePreferences:(id) sender {
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

    [prefs setObject:[server text] forKey:@"server"];
    [prefs setObject:[port text] forKey:@"port"];
    [prefs setBool:[wifi isOn] forKey:@"wifi"];
	[prefs setBool:[loadIndicator isOn] forKey:@"loadindicator"];
    [prefs setInteger:(int)([connectionIntervalSlider value] + 0.5f) forKey:@"connectionInterval"];
    [prefs setInteger:(int)([pollIntervalSlider value] + 0.5f) forKey:@"pollInterval"];    
	[prefs synchronize];
    
    [self.tabBarController setSelectedIndex:0];

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
