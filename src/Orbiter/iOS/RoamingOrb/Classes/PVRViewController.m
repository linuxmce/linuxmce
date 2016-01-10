//
//  PVRViewController.m
//  RoamingOrb
//
//  Created by Serge Wagener on 13/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PVRViewController.h"

@implementation PVRViewController
@synthesize webView, url;
@synthesize swipeUpRecognizer, swipeDownRecognizer, tabBar;

CGSize screenSize;
BOOL _isTabbarVisible = YES;

- (void)viewDidLoad {
	[super viewDidLoad];
	screenSize = [[UIScreen mainScreen] bounds].size;
/* 
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
*/
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	if (![prefs valueForKey:@"server"]) {
        NSLog(@"Server is not defined");
        [prefs setObject:@"192.168.80.1" forKey:@"server"];
    }
    url = @"http://%@/mythweb",[prefs objectForKey:@"server"];
    NSURLRequest *req = [NSURLRequest requestWithURL:[NSURL URLWithString:url]];
	[webView loadRequest:req];
}

- (void)viewWillAppear:(BOOL)animated {
    /*if(_isTabbarVisible) {
        NSLog(@"Orbiter shown, tabbar will hide ...");
        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDuration:0.55];
        
        CGRect tabBarFrame = self.tabBarController.tabBar.frame;
        [tabBar setAlpha:0.0];
        _isTabbarVisible=FALSE;
        tabBarFrame.origin.y = tabBarFrame.origin.y + tabBarFrame.size.height;
        self.tabBarController.tabBar.frame = tabBarFrame;
        [UIView commitAnimations];
    }*/
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations
	if(interfaceOrientation == UIInterfaceOrientationLandscapeRight)
		return YES;
	if(interfaceOrientation == UIInterfaceOrientationLandscapeLeft)
		return YES;
	return NO;
}

- (BOOL)           webView:(UIWebView *)webView
shouldStartLoadWithRequest:(NSURLRequest *)request
            navigationType:(UIWebViewNavigationType)navigationType
{
    if (_sessionChecked) {
        NSLog(@"session already checked.");
        return YES;
    }
    
    NSLog(@"will check session.");
    
    NSURLConnection *conn = [NSURLConnection connectionWithRequest:request delegate:self];
    if (conn == nil) {
        NSLog(@"cannot create connection");
    }
    return NO;
}

- (void) connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
        _sessionChecked = YES;
        
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *) response;
        int status = [httpResponse statusCode];
        
        if (status == 404) {
            NSLog(@"PVR backend not installed");
            [connection cancel];
            
            // get localized path for file from app bundle
            NSString *path;
            NSBundle *thisBundle = [NSBundle mainBundle];
            path = [thisBundle pathForResource:@"nopvr" ofType:@"html"];
            
            // make a file: URL out of the path
            NSURL *nopvrURL = [NSURL fileURLWithPath:path];
            [webView loadRequest:[NSURLRequest requestWithURL:nopvrURL]];
         }
        else {
            NSLog(@"PVR backend exists, continue loading");
            NSURLRequest *req = [NSURLRequest requestWithURL:[NSURL URLWithString:url]];
            [webView loadRequest:req];
        }
    }
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{    
    NSLog(@"Connection to PVR backend not possible");
    [connection cancel];
    
    // We load from internal bundled file, so ignore 404 check
    _sessionChecked = YES;
    
    // get localized path for file from app bundle
    NSString *path;
    NSBundle *thisBundle = [NSBundle mainBundle];
    path = [thisBundle pathForResource:@"nopvr" ofType:@"html"];
    
    // make a file: URL out of the path
    NSURLRequest *req = [NSURLRequest requestWithURL:[NSURL fileURLWithPath:path]];
	[webView loadRequest:req];
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
	[super didReceiveMemoryWarning];

	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	[super viewDidUnload];
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)dealloc {
	[super dealloc];
}

@end
