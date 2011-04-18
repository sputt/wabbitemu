//
//  WCMultiLineTextFieldCell.h
//  customcelltest
//
//  Created by William Towe on 4/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <AppKit/NSTextFieldCell.h>


@interface WCDoEverythingTextFieldCell : NSTextFieldCell <NSCopying> {
@private
    NSString *_secondaryTitle;
	NSImage *_icon;
	NSSize _iconSize;
	NSUInteger _badgeCount;
}
@property (copy,nonatomic) NSString *secondaryTitle;
@property (retain,nonatomic) NSImage *icon;
@property (assign,nonatomic) NSSize iconSize;
@property (assign,nonatomic) NSUInteger badgeCount;
@end
