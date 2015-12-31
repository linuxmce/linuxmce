//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import <Foundation/Foundation.h>
#import "UIDeviceAdditions.h"

// reactivate this as soon as iPad will be iOS4
@interface proxy_orbiter_client : NSObject <NSStreamDelegate> {
//@interface proxy_orbiter_client : NSObject  {
	UIImage *orbiterImage;
	NSTimer *pollTimer;

	NSString *serverIp;
	NSInteger deviceId, pollInterval;
}

@property(nonatomic, retain) UIImage *orbiterImage;
@property(nonatomic, retain) NSString *server;

@property(assign) NSInteger port;
@property(assign) NSInteger pollInterval;

- (id) delegate;
- (id) init:(id)delegate;
- (void) setDelegate:(id)newDelegate;
- (void) connect;
- (void) poll;
- (void) downloadImage;
- (void) disconnect;
- (void) writeToServer:(NSString *) str;
- (void) sendTouch:(NSInteger) x y:(NSInteger) y;

@end

@interface NSObject (MyObjectDelegateMethod)
- (void) didConnect;
- (void) didReceiveNews;
- (void) didReceiveNewImage:(UIImage*)orbiterImage;
- (void) didReceiveConnectionError:(NSString *)errorMessage;
@end