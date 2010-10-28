/**
 * This header is generated by class-dump-z 0.2a.
 * class-dump-z is Copyright (C) 2009 by KennyTM~, licensed under GPLv3.
 *
 * Source: /System/Library/PrivateFrameworks/BackRow.framework/BackRow
 */


@class NSString;

@interface BRTimer : NSObject {
@private
	BOOL _cancel;	// 4 = 0x4
	NSString *_name;	// 8 = 0x8
	double _interval;	// 12 = 0xc
}
@property(assign) BOOL cancel;	// G=0x3164bb31; S=0x3164bb41; @synthesize=_cancel
+ (id)createTimerWithName:(id)name;	// 0x315918d9
- (id)initWithName:(id)name;	// 0x315918dd
// declared property getter: - (BOOL)cancel;	// 0x3164bb31
- (void)dealloc;	// 0x31591941
- (void)reset;	// 0x3164bb51
// declared property setter: - (void)setCancel:(BOOL)cancel;	// 0x3164bb41
@end

