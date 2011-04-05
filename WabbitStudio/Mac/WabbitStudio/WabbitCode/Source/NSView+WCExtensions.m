//
//  NSView+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSView+WCExtensions.h"
#import "WCDefines.h"


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
	if (![self shouldDrawBackgroundString] || ![self backgroundString] || ![[self backgroundString] length])
		return;
	
	static NSDictionary *bStringAttributes = nil;
	
	if (!bStringAttributes) {
		NSMutableParagraphStyle *style = [[[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[style setAlignment:NSCenterTextAlignment];
		/*
		 NSShadow *shadow = [[[NSShadow alloc] init] autorelease];
		 [shadow setShadowOffset:NSMakeSize(1.0, -1.0)];
		 [shadow setShadowBlurRadius:1.0];
		 */
		
		bStringAttributes = [[NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName,[NSColor alternateSelectedControlTextColor],NSForegroundColorAttributeName,/*shadow,NSShadowAttributeName,*/ nil] retain];
	}
	
	NSRect bounds = [self bounds];
	NSAttributedString *string = [[[NSAttributedString alloc] initWithString:[self backgroundString] attributes:bStringAttributes] autorelease];
	NSRect stringRect = [string boundingRectWithSize:bounds.size options:0];
	
	// create a centered rect around the bounding box of the string to draw our box
	bounds = WCCenteredRect(NSMakeRect(0.0, 0.0, stringRect.size.width+(kBackgroundStringXMargin*2.0), stringRect.size.height+(kBackgroundStringYMargin*2.0)), bounds);
	
	// use the same color as Xcode for now
	[[NSColor colorWithCalibratedWhite:0.6 alpha:1.0] setFill];
	// create a rounded rect and fill it
	[[NSBezierPath bezierPathWithRoundedRect:bounds xRadius:5.0 yRadius:5.0] fill];
	
	// center the string rect inside our box rect
	bounds = WCCenteredRect(stringRect, bounds);
	
	[string drawInRect:bounds];
}

- (NSArray *)keysForCoding; {
	return nil;
}
@end
