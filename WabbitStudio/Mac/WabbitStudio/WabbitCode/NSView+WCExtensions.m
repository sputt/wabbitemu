//
//  NSView+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSView+WCExtensions.h"
#import "WCDefines.h"
#import "NSShadow+MCAdditions.h"
#import "NSBezierPath+MCAdditions.h"


@implementation NSView (NSView_WCExtensions)
- (BOOL)shouldDrawBackgroundString; {
	return NO;
}
- (NSString *)backgroundString; {
	return nil;
}
#define kBackgroundStringXMargin 10.0
#define kBackgroundStringYMargin 8.0
- (void)drawBackgroundString; {
	// make sure we should draw and that we have something to draw if we are supposed to
	if (![self shouldDrawBackgroundString] ||
		[self backgroundString] == nil ||
		[[self backgroundString] length] == 0)
		return;
	
	static NSDictionary *bStringAttributes = nil;
	static NSShadow *kDropShadow = nil;
	static NSShadow *kInnerShadow = nil;
	if (!bStringAttributes) {
		NSMutableParagraphStyle *style = [[[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[style setAlignment:NSCenterTextAlignment];
		
		bStringAttributes = [[NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSRegularControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName,[NSColor alternateSelectedControlTextColor],NSForegroundColorAttributeName,/*shadow,NSShadowAttributeName,*/ nil] retain];
		kDropShadow = [[NSShadow alloc] initWithColor:[NSColor colorWithCalibratedWhite:1.0 alpha:0.95] offset:NSMakeSize(0, -1.0) blurRadius:1.0];
		kInnerShadow = [[NSShadow alloc] initWithColor:[NSColor colorWithCalibratedWhite:0.55 alpha:0.95] offset:NSMakeSize(0.0, -1.0) blurRadius:1.0];
	}
	
	NSRect bounds = [self bounds];
	bounds.size.height -= 1.0;
	bounds.origin.y += 1.0;
	NSAttributedString *string = [[[NSAttributedString alloc] initWithString:[self backgroundString] attributes:bStringAttributes] autorelease];
	NSRect stringRect = [string boundingRectWithSize:bounds.size options:0];
	
	// create a centered rect around the bounding box of the string to draw our box
	bounds = WCCenteredRect(NSMakeRect(0.0, 0.0, stringRect.size.width+(kBackgroundStringXMargin*2.0), stringRect.size.height+(kBackgroundStringYMargin*2.0)), bounds);
	
	NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:bounds xRadius:5.0 yRadius:5.0];
	
	// draw our shadow first
	[NSGraphicsContext saveGraphicsState];
	[kDropShadow set];
	[path fill];
	[NSGraphicsContext restoreGraphicsState];
	
	// use the same color as Xcode for now
	[[NSColor colorWithCalibratedWhite:0.6 alpha:1.0] setFill];
	[path fill];
	
	[path fillWithInnerShadow:kInnerShadow];
	
	// center the string rect inside our box rect
	bounds = WCCenteredRect(stringRect, bounds);
	
	[string drawInRect:bounds];
}

- (NSArray *)keysForCoding; {
	return nil;
}
@end
