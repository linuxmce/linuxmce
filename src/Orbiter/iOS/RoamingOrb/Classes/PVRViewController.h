//
//  PVRViewController.h
//  RoamingOrb
//
//  Created by Serge Wagener on 06/09/2011.
//  Copyright 2010 LinuxMCE. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface PVRViewController : UIViewController<UIWebViewDelegate> {
	IBOutlet UIWebView *webView;
    NSString *url;
    BOOL _sessionChecked;
}

@property (nonatomic, retain) UIWebView *webView;
@property (nonatomic, retain) NSString *url;
@property (nonatomic, retain) IBOutlet UITabBar *tabBar;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeUpRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeDownRecognizer;

@end
