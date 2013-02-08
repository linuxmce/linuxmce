//
//  AboutViewController.m
//  iTouchOrb
//
//  Created by Serge Wagener on 01/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "AboutViewController.h"


@implementation AboutViewController
@synthesize bundleInfo;

- (void)viewDidLoad {
	NSLog(@"AboutView starting up...");
	[bundleInfo setText:[NSString stringWithFormat:@"%@ v%@"
						 , [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleDisplayName"]
						 , [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"]]];

	[super viewDidLoad];
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
	[super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
	[super viewDidUnload];
}

- (void)dealloc {
	[super dealloc];
}

@end
