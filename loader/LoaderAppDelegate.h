#import <UIKit/UIKit.h>
#import "LoaderVC.h"

@interface LoaderAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow *window;
	LoaderVC *_loader;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

