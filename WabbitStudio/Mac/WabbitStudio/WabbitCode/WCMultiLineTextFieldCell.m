//
//  WCMultiLineTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCMultiLineTextFieldCell.h"
#import "WCDefines.h"


@interface WCMultiLineTextFieldCell ()
- (NSAttributedString *)_attributedStringForSecondaryTitle;
@end

@implementation WCMultiLineTextFieldCell

- (void)dealloc {
	[_secondaryTitle release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone {
	WCMultiLineTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_secondaryTitle = [_secondaryTitle copy];
	
	return copy;
}

- (NSRect)titleRectForBounds:(NSRect)theRect {
	NSRect titleRect = [self secondaryTitleRectForBounds:theRect];
	
	return NSMakeRect(NSMinX(theRect), NSMinY(theRect), NSWidth(theRect), NSHeight(theRect)-NSHeight(titleRect));
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
	if ([[self secondaryTitle] length] > 0) {
		NSRect rect = [self titleRectForBounds:cellFrame];
		NSRect stRect = [self secondaryTitleRectForBounds:cellFrame];
		NSAttributedString *attributedString = [self _attributedStringForSecondaryTitle];
		NSSize size = [attributedString size];
		
		[attributedString drawInRect:WCCenteredRect(NSMakeRect(NSMinX(rect), NSMinY(stRect), NSWidth(rect), size.height), NSMakeRect(NSMinX(rect), NSMinY(stRect), NSWidth(rect), NSHeight(stRect)))];
	}
	
	[super drawInteriorWithFrame:cellFrame inView:controlView];
}

- (NSRect)secondaryTitleRectForBounds:(NSRect)bounds; {
	if ([[self secondaryTitle] length] == 0)
		return NSZeroRect;
	
	NSRect top, bottom;
	NSDivideRect(bounds, &top, &bottom, floor(NSHeight(bounds)/2.0), NSMinYEdge);
	
	return bottom;
}
		 
@synthesize secondaryTitle=_secondaryTitle;

- (NSAttributedString *)_attributedStringForSecondaryTitle {
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setLineBreakMode:NSLineBreakByTruncatingTail];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[self font],NSFontAttributeName,([self isHighlighted])?[NSColor alternateSelectedControlTextColor]:[NSColor disabledControlTextColor],NSForegroundColorAttributeName,style,NSParagraphStyleAttributeName, nil];
	NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[self secondaryTitle] attributes:attributes] autorelease];
	
	return attributedString;
}

@end
