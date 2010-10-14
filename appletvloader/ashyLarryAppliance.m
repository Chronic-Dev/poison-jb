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
	P0isonedMenuLauncherController *menuController = [[P0isonedMenuLauncherController alloc] init];
	NSLog(@"[BRApplicationStackManager singleton] : %@",[BRApplicationStackManager singleton]);
	NSLog(@"[[BRApplicationStackManager singleton] stack] : %@",[[BRApplicationStackManager singleton] stack]);
	//NSLog(@"self: %@ superclass: %@", self, super);
	[[[BRApplicationStackManager singleton] stack] pushController:menuController];
	//[[self stack] pushController:menuController];
	//return menuController;
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
					[BRApplianceCategory categoryWithName:@"Install SSH" identifier:@"ssh" preferredOrder:0],
					nil
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
