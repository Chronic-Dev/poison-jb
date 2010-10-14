//
//  P0isonedMenuLauncherController.h
//  greenp0ison
//
//  Created by Kevin Bradley on 10/13/10.

#import <Foundation/Foundation.h>
#import "BackRow/BackRow.h"

@interface P0isonedMenuLauncherController : BRMediaMenuController {
	NSMutableArray *_menuItems;
}
//list provider
- (float)heightForRow:(long)row;
- (long)itemCount;
- (id)itemForRow:(long)row;
- (BOOL)rowSelectable:(long)selectable;
- (id)titleForRow:(long)row;

@end
