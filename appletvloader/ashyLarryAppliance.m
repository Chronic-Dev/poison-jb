#import <QuartzCore/QuartzCore.h> 
#import <Foundation/Foundation.h>
#import "BackRow/BackRow.h"
#import "P0isonedMenuLauncherController.h"

@interface TopShelfController : NSObject 
{
	
}

- (void)selectCategoryWithIdentifier:(id)identifier;
- (id)topShelfView;

@end

@implementation TopShelfController

- (void)initWithApplianceController:(id)applianceController
{
	
}

- (void)selectCategoryWithIdentifier:(id)identifier
{
	
	P0isonedMenuLauncherController *menuController = [[P0isonedMenuLauncherController alloc] init];
	NSLog(@"[BRApplicationStackManager singleton] : %@",[BRApplicationStackManager singleton]);
	NSLog(@"[[BRApplicationStackManager singleton] stack] : %@",[[BRApplicationStackManager singleton] stack]);
	//NSLog(@"self: %@ superclass: %@", self, super);
	[[[BRApplicationStackManager singleton] stack] pushController:menuController];
	//[[self stack] pushController:menuController];
	//return menuController;
}

- (id)topShelfView
{
	id topShelf = [[BRTopShelfView alloc] init];
	return topShelf;
}

@end


@interface ashyLarryAppliance: BRBaseAppliance

@end

@implementation ashyLarryAppliance

+ (void)load
{

	//CGRect screenRect = [[UIScreen mainScreen] bounds];
	//Class brwin = NSClassFromString( @"BRWindow" );
	//id theLayer = [[brwin rootLayer] superclass];
	//NSLog(@"theLayer = %@", theLayer);

}

+ (void)initialize {
	NSLog(@"[ashyLarry initialize]");
}

- (id)applianceCategories {

	BRApplianceCategory *sshCat = [BRApplianceCategory categoryWithName: @"Install SSH" identifier: @"ssh" preferredOrder: 0];
	
	NSArray *applianceArray = [NSArray arrayWithObjects:sshCat,nil];
	return applianceArray;
}

- (id)identifierForContentAlias:(id)contentAlias {
	return @"ashyLarry";
}



- (id)topShelfController {
	//NSLog(@"%@ %s", self, _cmd);
	TopShelfController *shelfControl = [[TopShelfController alloc] init];
	return shelfControl;
}

- (id)selectCategoryWithIdentifier:(id)ident
{
	NSLog(@"selecteCategoryWithIdentifier: %@", ident);
	return nil;
}

- (BOOL)handleObjectSelection:(id)fp8 userInfo:(id)fp12
{
	NSLog(@"handleObjectSeection");
	return YES;
}

- (id)applianceSpecificControllerForIdentifier:(id)arg1 args:(id)arg2 { 

	NSLog(@"applianceSpecificControllerForIdentifier: %@ args: %@", arg1, arg2);
	return nil;
}

- (id)localizedSearchTitle { return @"ashyLarry"; }
- (id)applianceName { return @"ashyLarry (a)"; }
- (id)moduleName { return @"ashyLarry (m)"; }
- (id)applianceKey { return @"ashyLarry (m)"; }

@end





// vim:ft=objc
