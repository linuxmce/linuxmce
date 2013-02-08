

#import <UIKit/UIKit.h>


@interface ConfigViewController : UIViewController<UIScrollViewDelegate> {
	IBOutlet UIScrollView *scrollView;
	IBOutlet UILabel *pollIntervalLabel, *connectionIntervalLabel;
	IBOutlet UISlider *pollIntervalSlider, *connectionIntervalSlider;

	IBOutlet UISwitch *wifi;
	IBOutlet UISwitch *loadIndicator;
	
	IBOutlet UINavigationController *navigator;
}

@property (nonatomic, retain) IBOutlet UIScrollView *scrollView;
@property (nonatomic, retain) IBOutlet UILabel *pollIntervalLabel;
@property (nonatomic, retain) IBOutlet UILabel *connectionIntervalLabel;
@property (nonatomic, retain) IBOutlet UISlider *pollIntervalSlider;
@property (nonatomic, retain) IBOutlet UISlider *connectionIntervalSlider;
@property (nonatomic, retain) IBOutlet UITextField *server;
@property (nonatomic, retain) IBOutlet UITextField *port;

-(IBAction) pollSliderChanged:(id) sender;
-(IBAction) connectionSliderChanged:(id) sender;
-(IBAction) savePreferences:(id) sender;
-(IBAction) onExit: (id)sender;

@end
