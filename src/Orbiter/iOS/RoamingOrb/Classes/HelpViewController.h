//
//  HelpViewController.h
//  RoamingOrb
//
//  Created by Serge Wagener on 13/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface HelpViewController : UIViewController {
	IBOutlet UIWebView *webView;

}

@property (nonatomic, retain) UIWebView *webView;

@end
