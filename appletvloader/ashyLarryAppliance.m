#import <QuartzCore/QuartzCore.h> 
#import <Foundation/Foundation.h>
#import "P0isonedMenuLauncherController.h"

@interface TopShelfController : NSObject {
}
- (void)selectCategoryWithIdentifier:(id)identifier;
- (id)topShelfView;
@end

@implementation TopShelfController
- (void)initWithApplianceController:(id)applianceController {

}

- (void)selectCategoryWithIdentifier:(id)identifier {
	
	id menuController = nil;
	
	if ([identifier isEqualToString:@"software"])
	{
		
		menuController = [[P0isonedMenuLauncherController alloc] init];
		
	} else if ([identifier isEqualToString: @"about"])
	{
		NSString * path = [[NSBundle bundleForClass:[P0isonedMenuLauncherController class]] pathForResource:@"About" ofType:@"txt"];
		BRScrollingTextControl *textControls = [[BRScrollingTextControl alloc] init];
		[textControls setDocumentPath:path encoding:NSUTF8StringEncoding];
		NSString *myTitle = @"About greenpois0n";
		[textControls setTitle:myTitle];
		[textControls autorelease];
		menuController =  [BRController controllerWithContentControl:textControls];
	}
	
	[[[BRApplicationStackManager singleton] stack] pushController:menuController];
	
}

- (id)topShelfView {
	id topShelf = [[BRTopShelfView alloc] init];
	return topShelf;
}
@end

@interface ashyLarryAppliance: BRBaseAppliance {
	TopShelfController *_topShelfController;
	NSArray *_applianceCategories;
}
@property(nonatomic, readonly, retain) id topShelfController;

@end

@implementation ashyLarryAppliance
@synthesize topShelfController = _topShelfController;

+ (void)initialize {
}

- (id)init {
	if((self = [super init]) != nil) {
		_topShelfController = [[TopShelfController alloc] init];
		_applianceCategories = [[NSArray alloc] initWithObjects:
					[BRApplianceCategory categoryWithName:@"Inject Software" identifier:@"software" preferredOrder:0],
					[BRApplianceCategory categoryWithName:@"About" identifier:@"about" preferredOrder:1],nil
				];
	
	} return self;
}

- (id)applianceCategories {
	return _applianceCategories;
}

- (id)identifierForContentAlias:(id)contentAlias {
	return @"ashyLarry";
}

- (id)selectCategoryWithIdentifier:(id)ident {
	NSLog(@"selecteCategoryWithIdentifier: %@", ident);
	return nil;
}

- (BOOL)handleObjectSelection:(id)fp8 userInfo:(id)fp12 {
	NSLog(@"handleObjectSeection");
	return YES;
}

- (id)applianceSpecificControllerForIdentifier:(id)arg1 args:(id)arg2 {
	NSLog(@"applianceSpecificControllerForIdentifier: %@ args: %@", arg1, arg2);
	return nil;
}

- (id)localizedSearchTitle { return @"ashyLarry"; }
- (id)applianceName { return @"ashyLarry"; }
- (id)moduleName { return @"ashyLarry"; }
- (id)applianceKey { return @"ashyLarry"; }

@end





// vim:ft=objc
