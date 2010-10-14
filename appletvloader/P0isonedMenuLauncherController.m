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

	
	GPMedia *currentAsset = [[GPMedia alloc] init];
	[currentAsset setTitle:[_menuItems objectAtIndex:item]];
	BRImage *ourImage = [BRImage imageWithURL:[NSURL URLWithString:@"http://nitosoft.com/ATV2/gp/gp.png"]];
	//NSString *currentURL = [[updateArray objectAtIndex:(item-6)] valueForKey:@"imageUrl"];
	//NSString *currentVersion = [[updateArray objectAtIndex:(item-6)] valueForKey:@"version"];
	NSString *description = @"This sodomizes your couch in a surly fashion";
	//if ([[[updateArray objectAtIndex:(item-6)] allKeys] containsObject:@"description"])
	//	description = [[updateArray objectAtIndex:(item-6)] valueForKey:@"description"];
	[currentAsset setCoverArt:ourImage];
	//[currentAsset setCoverArt:[BRImage imageWithURL:[NSURL URLWithString:currentURL]]];
	NSMutableArray *customKeys = [[NSMutableArray alloc] init];
	NSMutableArray *customObjects = [[NSMutableArray alloc] init];
	
	[customKeys addObject:@"Version"];
	[customObjects addObject:@"4.2.0"];
	if(description != nil)
	{
		[currentAsset setSummary:description];
	}
	[currentAsset setCustomKeys:[customKeys autorelease] 
					 forObjects:[customObjects autorelease]];
	
	
	GPMediaPreview *preview = [[GPMediaPreview alloc]init];
	[preview setAsset:currentAsset];
	[preview setShowsMetadataImmediately:YES];
	[currentAsset release];
	return [preview autorelease];
}

- (id)oldpreviewControlForItem:(long)item
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
