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

- (void)viewDidLoad {
	[super viewDidLoad];
    url = @"http://192.168.80.1/mythweb";
    NSURLRequest *req = [NSURLRequest requestWithURL:[NSURL URLWithString:url]];
    //[webView setDelegate:self];
	[webView loadRequest:req];
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
        //[self log:@"session already checked."];
        return YES;
    }
    
    //[self log:@"will check session."];
    
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
