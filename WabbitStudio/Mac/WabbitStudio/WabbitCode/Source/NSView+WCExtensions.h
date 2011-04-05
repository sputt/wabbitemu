//
//  NSView+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 3/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSView.h>


@interface NSView (NSView_WCExtensions)
// lets NSView subclasses specify a string to draw under a certain condition; like Xcode draws in its views when they are empty
// the view just needs to call drawBackgroundString when it wants the string to be drawn, the method will check
// shouldDrawBackgroundString each time before actually drawing the string
- (BOOL)shouldDrawBackgroundString;
- (NSString *)backgroundString;
- (void)drawBackgroundString;

// to suppress a compiler warning in TLAnimatingOutlineView
- (NSArray *)keysForCoding;
@end
