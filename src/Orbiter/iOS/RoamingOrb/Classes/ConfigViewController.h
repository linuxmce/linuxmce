

#import <UIKit/UIKit.h>


@interface ConfigViewController : UIViewController<UIScrollViewDelegate> {
	IBOutlet UIScrollView *scrollView;
	IBOutlet UILabel *pollIntervalLabel;
	IBOutlet UISlider *pollIntervalSlider;

	IBOutlet UISwitch *wifi;
	IBOutlet UISwitch *loadIndicator;
	
	IBOutlet UINavigationController *navigator;
}

@property (nonatomic, retain) IBOutlet UIScrollView *scrollView;
@property (nonatomic, retain) IBOutlet UILabel *pollIntervalLabel;
@property (nonatomic, retain) IBOutlet UISlider *pollIntervalSlider;

-(IBAction) sliderChanged:(id) sender;
-(IBAction) savePreferences:(id) sender;
-(IBAction) onExit: (id)sender;

@end
