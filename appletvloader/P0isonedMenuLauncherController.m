//
//  P0isonedMenuLauncherController.m
//  greenp0ison
//
//  Created by Kevin Bradley on 10/13/10.


#import "P0isonedMenuLauncherController.h"
#import <Foundation/Foundation.h>
#import "BackRow/BackRow.h"

@implementation P0isonedMenuLauncherController
- (id)init {
	if((self = [super init]) != nil) {
		_menuItems = [[NSMutableArray alloc] init];

		[_menuItems addObject:@"Fuck"];
		[_menuItems addObject:@"Your"];
		[_menuItems addObject:@"Couch"];
		[_menuItems addObject:@"AppleTV2,1"];
		[self setTitle:@"greenp0ison in yo' face!"];
		//[self doSSHGoodness];
		[[self list] setDatasource:self];
	} return self;
}

- (id)previewControlForItem:(long)item
{
	BRImageAndSyncingPreviewController *previewController = [[BRImageAndSyncingPreviewController alloc] init];
	BRImage *sampleImage = [[BRThemeInfo sharedTheme] gearImage];
	//BRImage *image = [[BRImage imageWithPath:@""];
	[previewController setImage:sampleImage];
	return previewController;
}

- (void)itemSelected:(long)selected; {
	NSLog(@"itemSelected: %@", [_menuItems objectAtIndex:selected]);
}

- (void)doSSHGoodness {
	NSLog(@"doSSHGoodness");
	NSLog(@"im goin' from ashy to classy");
}

- (float)heightForRow:(long)row {
	return 0.0f;
}

- (long)itemCount {
	return [_menuItems count];
}

- (id)itemForRow:(long)row {
	if(row > [_menuItems count])
		return nil;

	//NSLog(@"%@ %s", self, _cmd);
	BRMenuItem * result = [[BRMenuItem alloc] init];
	NSString *theTitle = [_menuItems objectAtIndex: row];

	[result setText:theTitle withAttributes:[[BRThemeInfo sharedTheme] menuTitleTextAttributes]];

	return result;
}

- (BOOL)rowSelectable:(long)selectable {
	return TRUE;
}

- (id)titleForRow:(long)row {
	return [_menuItems objectAtIndex:row];
}

@end
