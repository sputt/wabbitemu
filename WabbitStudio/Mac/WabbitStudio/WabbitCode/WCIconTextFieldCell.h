//
//  WCIconTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 4/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCMultiLineTextFieldCell.h"


@interface WCIconTextFieldCell : WCMultiLineTextFieldCell <NSCopying> {
@private
    NSImage *_icon; // always on the left edge
	NSSize _iconSize; // default is 16x16
	BOOL _centerIcon; // default to YES, centers icon within its frame
}
@property (retain,nonatomic) NSImage *icon;
@property (assign,nonatomic) NSSize iconSize;
@property (assign,nonatomic) BOOL centerIcon;

- (NSRect)iconRectForBounds:(NSRect)bounds remainingRect:(NSRectPointer)remainingRect;
@end
