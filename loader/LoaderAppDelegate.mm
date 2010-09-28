#import "LoaderAppDelegate.h"

#include <notify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <objc/runtime.h>
#import "LSApplicationWorkspace.h"

// uicache - start
// by saurik. thanks!
@interface NSMutableArray (Cydia)
- (void) addInfoDictionary:(NSDictionary *)info;
@end

@implementation NSMutableArray (Cydia)

- (void) addInfoDictionary:(NSDictionary *)info {
    [self addObject:info];
}

@end

@interface NSMutableDictionary (Cydia)
- (void) addInfoDictionary:(NSDictionary *)info;
@end

@implementation NSMutableDictionary (Cydia)

- (void) addInfoDictionary:(NSDictionary *)info {
    NSString *bundle = [info objectForKey:@"CFBundleIdentifier"];
    [self setObject:info forKey:bundle];
}

@end

void _uicache() {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSString *path = @"/var/mobile/Library/Caches/com.apple.mobile.installation.plist";

    Class $LSApplicationWorkspace(objc_getClass("LSApplicationWorkspace"));
    LSApplicationWorkspace *workspace($LSApplicationWorkspace == nil ? nil : [$LSApplicationWorkspace defaultWorkspace]);

    if (NSMutableDictionary *cache = [NSMutableDictionary dictionaryWithContentsOfFile:path]) {
        NSFileManager *manager = [NSFileManager defaultManager];
        NSError *error = nil;

        NSMutableArray *bundles([NSMutableArray arrayWithCapacity:16]);

        id system = [cache objectForKey:@"System"];
        if (system == nil)
            goto error;

        [system removeAllObjects];

        if (NSArray *apps = [manager contentsOfDirectoryAtPath:@"/Applications" error:&error]) {
            for (NSString *app in apps)
                if ([app hasSuffix:@".app"]) {
                    NSString *path = [@"/Applications" stringByAppendingPathComponent:app];
                    NSString *plist = [path stringByAppendingPathComponent:@"Info.plist"];

                    if (NSMutableDictionary *info = [NSMutableDictionary dictionaryWithContentsOfFile:plist]) {
                        NSString *bundle;
			if((bundle = [info objectForKey:@"CFBundleIdentifier"])) {
                            [bundles addObject:path];
                            [info setObject:path forKey:@"Path"];
                            [info setObject:@"System" forKey:@"ApplicationType"];
                            [system addInfoDictionary:info];
                        } else
                            fprintf(stderr, "%s missing CFBundleIdentifier", [app UTF8String]);
                    }
                }
        } else goto error;

        [cache writeToFile:path atomically:YES];

        if (workspace != nil)
            for (NSString *bundle in bundles) {
                [workspace unregisterApplication:[NSURL fileURLWithPath:bundle]];
                [workspace registerApplication:[NSURL fileURLWithPath:bundle]];
            }

        if (false) error:
            fprintf(stderr, "%s\n", error == nil ? strerror(errno) : [[error localizedDescription] UTF8String]);
    } else fprintf(stderr, "cannot open cache file. incorrect user?\n");

    unlink([[NSString stringWithFormat:@"/var/mobile/Library/Caches/com.apple.springboard-imagecache-icons"] UTF8String]);
    unlink([[NSString stringWithFormat:@"/var/mobile/Library/Caches/com.apple.springboard-imagecache-icons.plist"] UTF8String]);

    unlink([[NSString stringWithFormat:@"/var/mobile/Library/Caches/com.apple.springboard-imagecache-smallicons"] UTF8String]);
    unlink([[NSString stringWithFormat:@"/var/mobile/Library/Caches/com.apple.springboard-imagecache-smallicons.plist"] UTF8String]);

    system([[NSString stringWithFormat:@"rm -rf /var/mobile/Library/Caches/SpringBoardIconCache"] UTF8String]);
    system([[NSString stringWithFormat:@"rm -rf /var/mobile/Library/Caches/SpringBoardIconCache-small"] UTF8String]);

    system([[NSString stringWithFormat:@"rm -rf /var/mobile/Library/Caches/com.apple.IconsCache"] UTF8String]);

    notify_post("com.apple.mobile.application_installed");

    [pool release];
}

// uicache - end

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
	_uicache();
	sleep(1);
}

- (void)dealloc {
	[_loader release];
	[window release];
	[super dealloc];
}


@end
