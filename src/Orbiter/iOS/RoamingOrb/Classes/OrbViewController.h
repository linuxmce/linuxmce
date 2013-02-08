

#import <UIKit/UIKit.h>
#import "proxy-orbiter-client.h"
#import "reachability.h"
#import "MBProgressHUD.h"


@interface OrbViewController : UIViewController<MBProgressHUDDelegate> {
	IBOutlet UIImageView *orbView;
	IBOutlet UITabBar *tabBar;

	NSTimer *pollTimer, *connectTimer;
	NSInteger pollInterval, connectInterval;

	MBProgressHUD *HUD;

	UISwipeGestureRecognizer *swipeUpRecognizer, *swipeDownRecognizer, *swipeLeftRecognizer, *swipeRightRecognizer;
}

@property (nonatomic, retain) IBOutlet UIImageView *orbView;
@property (nonatomic, retain) IBOutlet UITabBar *tabBar;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeUpRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeDownRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeLeftRecognizer;
@property (nonatomic, retain) UISwipeGestureRecognizer *swipeRightRecognizer;

- (void)showTabBar:(BOOL)visible;
//- (void)didReceiveNews;
- (void)didReceiveNewImage:(UIImage*)orbiterImage;
- (void)didReceiveConnectionError:(NSString *)errorMessage;
- (CGFloat) mainScreenScale;
- (BOOL) isIPad;
- (void)startPolling;
- (void)stopPolling;
- (void)startConnecting;
- (void)stopConnecting;

@end
