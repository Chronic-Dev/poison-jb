//
//  main.m
//  greenpois0n
//
//  Created by Ari Weinstein on 7/02/09.
//  Copyright Squish Software 2009. All rights reserved.
//

#include <libpois0n.h>
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#define OPAQUE_HEXCOLOR(c) [NSColor colorWithDeviceRed:((c>>16)&0xFF)/255.0 \
green:((c>>8)&0xFF)/255.0 \
blue:(c&0xFF)/255.0 \
alpha:1.0]

NSButton *jailbreakButton;
NSButton *resetButton;
NSProgressIndicator *progressIndicator;
NSTextField *secondsLabel;
NSTextField *secondsTextLabel;
NSTextField *firstLabel;
NSTextField *secondLabel;
NSTextField *thirdLabel;
NSTextField *fourthLabel;
NSImageView *greenpois0nLogo;
NSBox *mainBox;
void buildMenus(NSString *appName);
void labelIfy(NSTextField *textField);
BOOL reset = false;

@interface Callback : NSObject {}
@end
@implementation Callback
- (void)reset {
	reset = true;
	
	[secondsLabel setStringValue:@"5"];
	[firstLabel setFont:[NSFont fontWithName:@"Lucida Grande Bold" size:12.0]];
	[secondLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[thirdLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[fourthLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
}
- (void)callback {
	[resetButton setEnabled:TRUE];
	[jailbreakButton setEnabled:NO];
	[jailbreakButton setTitle:@"Waiting for DFU..."];
	[NSBlockOperation blockOperationWithBlock:^{
		while(pois0n_is_ready()) {
			sleep(1);
		}
		
		if(!pois0n_is_compatible()) {
			pois0n_inject();
		}
	}];

	[mainBox addSubview:secondsLabel];
	[mainBox addSubview:secondsTextLabel];
	[[greenpois0nLogo animator] setAlphaValue:0.0];
	[[secondsLabel animator] setAlphaValue:1.0];
	[[secondsTextLabel animator] setAlphaValue:1.0];
	
	[secondsLabel setStringValue:@"5"];
	[firstLabel setFont:[NSFont fontWithName:@"Lucida Grande Bold" size:12.0]];
	[self performSelector:@selector(stage2) withObject:nil afterDelay:1.0];
}
- (void)stage2 {
	if (reset) {
		reset = false;
		[self callback];
		return;
	}
	int current = [[secondsLabel stringValue] intValue];
	if (current != 1) {
		[secondsLabel setStringValue:[NSString stringWithFormat:@"%d", current-1]];
		[self performSelector:@selector(stage2) withObject:nil afterDelay:1.0];
	} else {
		[secondsLabel setStringValue:@"3"];
		[firstLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
		[secondLabel setFont:[NSFont fontWithName:@"Lucida Grande Bold" size:12.0]];
		[self performSelector:@selector(stage3) withObject:nil afterDelay:1.0];
	}
}
- (void)stage3 {
	if (reset) {
		reset = false;
		[self callback];
		return;
	}
	int current = [[secondsLabel stringValue] intValue];
	if (current != 1) {
		[secondsLabel setStringValue:[NSString stringWithFormat:@"%d", current-1]];
		[self performSelector:@selector(stage3) withObject:nil afterDelay:1.0];
	} else {
		[secondsLabel setStringValue:@"10"];
		[secondLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
		[thirdLabel setFont:[NSFont fontWithName:@"Lucida Grande Bold" size:12.0]];
		[self performSelector:@selector(stage4) withObject:nil afterDelay:1.0];
	}
}
- (void)stage4 {
	if (reset) {
		reset = false;
		[self callback];
		return;
	}
	int current = [[secondsLabel stringValue] intValue];
	if (current != 1) {
		[secondsLabel setStringValue:[NSString stringWithFormat:@"%d", current-1]];
		[self performSelector:@selector(stage4) withObject:nil afterDelay:1.0];
	} else {
		[secondsLabel setStringValue:@"10"];
		[thirdLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
		[fourthLabel setFont:[NSFont fontWithName:@"Lucida Grande Bold" size:12.0]];
		[self performSelector:@selector(stage5) withObject:nil afterDelay:1.0];
	}
}
- (void)stage5 {
	if (reset) {
		reset = false;
		[self callback];
		return;
	}
	int current = [[secondsLabel stringValue] intValue];
	if (current != 0) {
		[secondsLabel setStringValue:[NSString stringWithFormat:@"%d", current-1]];
		[self performSelector:@selector(stage5) withObject:nil afterDelay:1.0];
	} else {
		[fourthLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
		[[greenpois0nLogo animator] setAlphaValue:1.0];
		[[secondsLabel animator] setAlphaValue:0.0];
		[[secondsTextLabel animator] setAlphaValue:0.0];
		[resetButton setEnabled:NO];
		[progressIndicator setIndeterminate:YES];
		[progressIndicator startAnimation:nil];
	}
}
@end

int main(int argc, char *argv[]) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[NSApplication sharedApplication];
	NSString *appName = @"greenpois0n";
	buildMenus(appName);
	
	pois0n_init();
	
	Callback *callback = [[Callback alloc] init];
	NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(250, 312, 480, 270) styleMask:NSClosableWindowMask|NSTitledWindowMask|NSMiniaturizableWindowMask backing:NSBackingStoreBuffered defer:NO];
	NSView *greenView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 420, 270)];
	NSTextField *poisonJBLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(111, 217, 257, 44)];
	NSTextField *instructionLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(17, 199, 446, 17)];
	secondsLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(340, 50, 76, 57)];
	secondsTextLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(348, 40, 56, 16)];
	firstLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 85, 317, 16)];
	secondLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 61, 317, 16)];
	thirdLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 37, 317, 16)];
	fourthLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 13, 317, 16)];
	NSTextField *copyrightLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(17, 11, 453, 13)];
	mainBox = [[NSBox alloc] initWithFrame:NSMakeRect(29, 67, 434, 138)];
	NSBox *dividerBox = [[NSBox alloc] initWithFrame:NSMakeRect(319, 7, 5, 98)];
	resetButton = [[NSButton alloc] initWithFrame:NSMakeRect(347, 12, 61, 17)];
	greenpois0nLogo = [[NSImageView alloc] initWithFrame:NSMakeRect(346, 38, 64, 64)];
	progressIndicator = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(164, 34, 298, 20)];

	[window setContentView:greenView];
	[window setTitle:appName];
	
	labelIfy(poisonJBLabel);
	labelIfy(instructionLabel);
	labelIfy(secondsLabel);
	labelIfy(secondsTextLabel);
	labelIfy(firstLabel);
	labelIfy(secondLabel);
	labelIfy(thirdLabel);
	labelIfy(fourthLabel);
	labelIfy(copyrightLabel);
	[poisonJBLabel setStringValue:appName];
	[poisonJBLabel setFont:[NSFont fontWithName:@"Helvetica Neue Light" size:32.0]];
	[instructionLabel setStringValue:@"Please turn off your device and connect it to begin."];
	[instructionLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:13.0]];
	[secondsLabel setStringValue:@"0"];
	[secondsLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:48.0]];
	[secondsTextLabel setStringValue:@"Seconds"];
	[secondsTextLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[firstLabel setStringValue:@"Get ready for DFU."];
	[firstLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[secondLabel setStringValue:@"Press and hold sleep button."];
	[secondLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[thirdLabel setStringValue:@"Continue holding sleep; press and hold home."];
	[thirdLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[fourthLabel setStringValue:@"Release sleep; continue holding home button."];
	[fourthLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:12.0]];
	[copyrightLabel setStringValue:@"© 2009-2010 Chronic Dev Team (http://chronic-dev.org). Beware copyright monster!"];
	[copyrightLabel setFont:[NSFont fontWithName:@"Lucida Grande" size:10.0]];
	
	[secondsLabel setAlphaValue:0.0];
	[secondsTextLabel setAlphaValue:0.0];
	[progressIndicator setIndeterminate:FALSE];
	[greenpois0nLogo setImage:[NSImage imageNamed:@"greenpois0n.icns"]];

	[[resetButton cell] setControlSize:NSMiniControlSize];
	[resetButton setBezelStyle:NSRoundRectBezelStyle];
	[resetButton setTitle:@"Reset"];
	[resetButton setFont:[NSFont fontWithName:@"Lucida Grande" size:9.0]];
	[resetButton setFocusRingType:NSFocusRingTypeNone];
	[resetButton setTarget:callback];
	[resetButton setAction:@selector(reset)];
	[resetButton setEnabled:FALSE];
	
	[mainBox addSubview:firstLabel];
	[mainBox addSubview:secondLabel];
	[mainBox addSubview:thirdLabel];
	[mainBox addSubview:fourthLabel];
	[mainBox addSubview:dividerBox];
	[mainBox addSubview:greenpois0nLogo];
	[mainBox addSubview:resetButton];
	[mainBox setTitle:@""];
	[dividerBox setTitle:@""];
	[dividerBox setBoxType:NSBoxSeparator];
	
	jailbreakButton = [[NSButton alloc] initWithFrame:NSMakeRect(26, 28, 138, 32)];
	[jailbreakButton setBezelStyle:NSRoundedBezelStyle];
	[jailbreakButton setTitle:@"Jailbreak"];
	[jailbreakButton setFont:[NSFont fontWithName:@"Lucida Grande" size:13.0]];
	[jailbreakButton setFocusRingType:NSFocusRingTypeNone];
	[jailbreakButton setTarget:callback];
	[jailbreakButton setAction:@selector(callback)];
	[greenView addSubview:jailbreakButton];
	[greenView addSubview:progressIndicator];
	[greenView addSubview:mainBox];
	[greenView addSubview:copyrightLabel];
	[greenView addSubview:instructionLabel];
	[greenView addSubview:poisonJBLabel];
	
	pois0n_exit();
	
	[window makeKeyAndOrderFront:nil];
	[window center];
    return NSApplicationMain(argc, (const char **)argv);
	[pool release];
}