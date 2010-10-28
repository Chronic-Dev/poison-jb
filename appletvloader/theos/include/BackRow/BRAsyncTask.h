/**
 * This header is generated by class-dump-z 0.2a.
 * class-dump-z is Copyright (C) 2009 by KennyTM~, licensed under GPLv3.
 *
 * Source: /System/Library/PrivateFrameworks/BackRow.framework/BackRow
 */


@class BRAsyncTaskContext, NSString, NSThread;

@interface BRAsyncTask : NSObject {
@private
	NSThread *_callingThread;	// 4 = 0x4
	id _target;	// 8 = 0x8
	SEL _selector;	// 12 = 0xc
	id _object;	// 16 = 0x10
	id _result;	// 20 = 0x14
	BRAsyncTaskContext *_context;	// 24 = 0x18
	int _state;	// 28 = 0x1c
	NSString *_identifier;	// 32 = 0x20
}
@property(readonly, retain) NSThread *callingThread;	// G=0x315b1355; converted property
@property(readonly, retain) BRAsyncTaskContext *context;	// G=0x3163bdb5; converted property
@property(retain) NSString *identifier;	// G=0x3163bdc5; S=0x3163bde5; converted property
@property(readonly, retain) id object;	// G=0x3163bda5; converted property
@property(readonly, retain) id result;	// G=0x315b3745; converted property
@property(readonly, assign) SEL selector;	// G=0x3163bd85; converted property
@property(assign) int state;	// G=0x315b1ab5; S=0x3163bdd5; converted property
@property(readonly, retain) id target;	// G=0x3163bd95; converted property
+ (void)initialize;	// 0x315b1029
+ (id)taskWithSelector:(SEL)selector onTarget:(id)target withObject:(id)object;	// 0x315b107d
+ (id)taskWithSelector:(SEL)selector onTarget:(id)target withObject:(id)object withContext:(id)context;	// 0x315b10b1
+ (id)voidReturnValue;	// 0x3163bd79
- (id)_initWithSelector:(SEL)selector onTarget:(id)target withObject:(id)object withContext:(id)context;	// 0x315b1105
// converted property getter: - (id)callingThread;	// 0x315b1355
- (void)cancel;	// 0x3163be1d
// converted property getter: - (id)context;	// 0x3163bdb5
- (void)dealloc;	// 0x315b3825
- (id)description;	// 0x3163be4d
// converted property getter: - (id)identifier;	// 0x3163bdc5
- (id)invoke;	// 0x315b1ac5
// converted property getter: - (id)object;	// 0x3163bda5
// converted property getter: - (id)result;	// 0x315b3745
- (void)run;	// 0x315b11bd
// converted property getter: - (SEL)selector;	// 0x3163bd85
// converted property setter: - (void)setIdentifier:(id)identifier;	// 0x3163bde5
// converted property setter: - (void)setState:(int)state;	// 0x3163bdd5
// converted property getter: - (int)state;	// 0x315b1ab5
// converted property getter: - (id)target;	// 0x3163bd95
@end

