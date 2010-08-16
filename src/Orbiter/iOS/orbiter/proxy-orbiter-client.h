//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import <Foundation/Foundation.h>


@interface proxy_orbiter_client : NSObject <NSStreamDelegate> {
	UIImage *orbiterImage;
	NSTimer *pollTimer;
}

@property(nonatomic, retain) UIImage *orbiterImage;

- (id) delegate;
- (id) init:(id)delegate;
- (void) setDelegate:(id)newDelegate;
- (void) connect:(NSString *) serverAddress port:(NSInteger) serverPort;
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