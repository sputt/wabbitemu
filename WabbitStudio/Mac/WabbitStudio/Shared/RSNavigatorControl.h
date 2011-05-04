//
//  RSNavigatorControl.h
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSControl.h>


@class RSNavigatorControl;

@protocol RSNavigatorDataSource <NSObject>
@required
- (NSUInteger)numberOfItemsInNavigatorControl:(RSNavigatorControl *)navigatorControl;
- (CGFloat)itemWidthForNavigatorControl:(RSNavigatorControl *)navigatorControl;
- (NSSize)imageSizeForNavigatorControl:(RSNavigatorControl *)navigatorControl;
- (NSImage *)navigatorControl:(RSNavigatorControl *)navigatorControl imageForItemAtIndex:(NSUInteger)itemIndex;
- (NSView *)navigatorControl:(RSNavigatorControl *)navigatorControl viewForItemAtIndex:(NSUInteger)itemIndex;
@optional
- (NSString *)navigatorControl:(RSNavigatorControl *)navigatorControl tooltipForItemAtIndex:(NSUInteger)itemIndex;
@end

@interface RSNavigatorControl : NSControl {
@private
    id <RSNavigatorDataSource> _dataSource;
	NSView *_swapView;
	
	NSUInteger _selectedItemIndex;
	NSUInteger _pressedItemIndex;
}
@property (assign,nonatomic) id <RSNavigatorDataSource> dataSource;
@property (assign,nonatomic) NSUInteger selectedItemIndex;
@property (assign,nonatomic) NSUInteger pressedItemIndex;
@property (assign,nonatomic) NSView *swapView;
@end