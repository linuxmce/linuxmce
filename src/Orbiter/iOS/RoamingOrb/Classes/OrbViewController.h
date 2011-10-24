

#import <UIKit/UIKit.h>
#import "proxy-orbiter-client.h"
#import "reachability.h"


@interface OrbViewController : UIViewController {
	IBOutlet UIImageView *orbView;
	IBOutlet UIActivityIndicatorView *activityIndicator;
	IBOutlet UITabBar *tabBar;

	NSTimer *pollTimer, *connectTimer;
	NSInteger pollInterval, connectInterval;

	UISwipeGestureRecognizer *swipeUpRecognizer, *swipeDownRecognizer, *swipeLeftRecognizer, *swipeRightRecognizer;
}

@property (nonatomic, retain) IBOutlet UIImageView *orbView;
@property (nonatomic, retain) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (nonatomic, retain) IBOutlet UITabBar *tabBar;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeUpRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeDownRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeLeftRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeRightRecognizer;

- (void)didReceiveNews;
- (void)didReceiveNewImage:(UIImage*)orbiterImage;
- (void)didReceiveConnectionError:(NSString *)errorMessage;
- (CGFloat) mainScreenScale;
- (BOOL) isIPad;

@end
