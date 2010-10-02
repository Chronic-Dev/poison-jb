#import "LoaderAppDelegate.h"

//#include <notify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <objc/runtime.h>

@implementation LoaderAppDelegate
@synthesize window;


#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	_loader = [[LoaderVC alloc] initWithStyle:UITableViewStyleGrouped];
	UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:_loader];
	
	[window addSubview:[nav view]];
	[window makeKeyAndVisible]; 
	[nav release];
	
	return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
	system("su mobile -c uicache");
	sleep(2);
	system("killall SpringBoard");
}

- (void)applicationWillResignActive:(UIApplication *)application {
	if([[[UIDevice currentDevice] systemVersion] intValue] >= 4) {
		// no backgrounding for you!
		system("su mobile -c uicache");
		sleep(2);
		system("killall SpringBoard");
	}
}

- (void)dealloc {
	[_loader release];
	[window release];
	[super dealloc];
}


@end
