//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import "orbiterAppDelegate.h"

@implementation orbiterAppDelegate

//@synthesize window;


//- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
- (void)applicationDidFinishLaunching:(UIApplication *)application {    
 	OrbiterView *viewController = [OrbiterView alloc];
	[window addSubview:viewController.view];
    [window makeKeyAndVisible];
}

#pragma mark -
#pragma mark Memory management


- (void)dealloc {
    //[mainViewController release];
    [window release];
    [super dealloc];
}

@end
