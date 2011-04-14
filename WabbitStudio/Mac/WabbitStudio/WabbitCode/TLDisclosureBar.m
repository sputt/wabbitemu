//
//  TLDisclosureBar.m
//  Created by Jonathan Dann and on 20/10/2008.
//	Copyright (c) 2008, espresso served here.
//	All rights reserved.
//
//	Redistribution and use in source and binary forms, with or without modification, 
//	are permitted provided that the following conditions are met:
//
//	Redistributions of source code must retain the above copyright notice, this list 
//	of conditions and the following disclaimer.
//
//	Redistributions in binary form must reproduce the above copyright notice, this list 
//	of conditions and the following disclaimer in the documentation and/or other materials 
//	provided with the distribution.
//
//	Neither the name of the espresso served here nor the names of its contributors may be
//	used to endorse or promote products derived from this software without specific prior 
//	written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
//	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
//	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
//	OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// If you use it, acknowledgement in an About Page or other appropriate place would be nice.
// For example, "Contains code from "TLAnimatingOutlineView" by Jonathan Dann http://code.google.com/p/tlanimatingoutlineview/" will do.

#import "TLDisclosureBar.h"

#import "TLCollapsibleView.h"

#define TL_DISCLOSURE_BAR_SUBVIEW_SPACING 6.0f
#define TL_DISCLOSURE_BAR_TEXT_WIDTH_PADDING 10.0f
#define TL_DISCLOSURE_BAR_MINX_PADDING 8.0f

@interface TLDisclosureBar ()
@property(readwrite,retain) NSButton *disclosureButton;
@property(readwrite,retain) NSImageView *imageViewLeft;
@property(readwrite,retain) NSTextField *labelField;
//@property(readwrite,retain) NSView *accessoryView;
@end

@interface TLDisclosureBar (Private)
- (void)_adjustSubviews;
- (void)_adjustSubviewsFollowingResize;
@end

@implementation TLDisclosureBar (Private)
- (void)_adjustSubviews;
{
	NSRect imageViewLeftFrame = [self.imageViewLeft frame];
	imageViewLeftFrame.origin.x = (self.hasDisclosureButton ? NSMaxX([self.disclosureButton frame]) : NSMinX([self frame])) + TL_DISCLOSURE_BAR_SUBVIEW_SPACING;
	[self.imageViewLeft setFrame:imageViewLeftFrame];
	
	NSRect labelFieldFrame = [self.labelField frame];
	labelFieldFrame.origin.x = NSMaxX(imageViewLeftFrame) + TL_DISCLOSURE_BAR_SUBVIEW_SPACING;
	labelFieldFrame.size.width = (self.accessoryView ? NSMinX([self.accessoryView frame]) : NSMaxX([self frame])) - NSMinX(imageViewLeftFrame);
	[self.labelField setFrame:labelFieldFrame];
}

- (void)_adjustSubviewsFollowingResize;
{
	NSSize labelSize = [[self.labelField stringValue] sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:[[self.labelField cell] controlSize]]],NSFontAttributeName,nil]];
	CGFloat limitingXCoordinate = self.accessoryView ? NSMinX([self.accessoryView frame]) : NSMaxX([self frame]);

	if (NSMinX([self.labelField frame]) + labelSize.width / 2.0 + TL_DISCLOSURE_BAR_SUBVIEW_SPACING > limitingXCoordinate) {
		[self.labelField setAutoresizingMask:NSViewNotSizable]; // To avoi the classic resising bug we stop autoresizing before the view's frame.size.width is reduced to zero. We reactiveate autoresizing in the else block.
		[self.labelField setHidden:YES];
	} else {
		// BUGFIX: even with changing the autoresizing mask, sometimes when the view is resized too quickly we get the classic autoresizing confusion bug so we set the frame of the label field to compensate.
		NSRect labelFieldFrame = [self.labelField frame];
		labelFieldFrame.origin.x = NSMaxX([self.imageViewLeft frame]) + TL_DISCLOSURE_BAR_SUBVIEW_SPACING;
		labelFieldFrame.size.width = limitingXCoordinate - NSMinX(labelFieldFrame);
		[self.labelField setFrame:labelFieldFrame];
		[self.labelField setAutoresizingMask:NSViewWidthSizable];
		[self.labelField setHidden:NO];
	}
	
	if (NSMaxX([self.imageViewLeft frame]) + TL_DISCLOSURE_BAR_SUBVIEW_SPACING > limitingXCoordinate)
		[self.imageViewLeft setHidden:YES];
	else
		[self.imageViewLeft setHidden:NO];
}

@end

@implementation TLDisclosureBar
@synthesize disclosureButton = _disclosureButton;
@synthesize imageViewLeft = _imageViewLeft;
@synthesize labelField = _labelField;
@dynamic hasDisclosureButton;

- (id)initWithFrame:(NSRect)frame;
{
	[NSException raise:NSGenericException format:@"%s is not the designated initialiser for instances of class: %@",__func__,[self className]];
	return nil;
}

- (id)initWithFrame:(NSRect)frame expanded:(BOOL)expanded;
{
	if (![super initWithFrame:frame])
		return nil;
	
	self.drawsBorder = YES;
	self.borderSidesMask = (TLMinYEdge|TLMaxYEdge);
	self.drawsHighlight = YES;
	[self setAutoresizesSubviews:YES];
	[self setAutoresizingMask:NSViewWidthSizable];
	
	self.activeFillGradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],[NSColor colorWithCalibratedWhite:0.814 alpha:1.0],nil]] autorelease];
	self.inactiveFillGradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],nil]] autorelease];
	self.clickedFillGradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:[NSColor colorWithCalibratedWhite:0.83f alpha:1.0f],nil]] autorelease];
	
	NSRect disclosureFrame = frame;
	disclosureFrame.origin.x += TL_DISCLOSURE_BAR_MINX_PADDING;
	disclosureFrame.size.width = 10.0f;
	NSButton *disclosureButton = [[[NSButton alloc] initWithFrame:disclosureFrame] autorelease];
	self.disclosureButton = disclosureButton;
	[self.disclosureButton setButtonType:NSOnOffButton];
	[self.disclosureButton setBezelStyle:NSDisclosureBezelStyle];
	[self.disclosureButton setTitle:@""];
	[self.disclosureButton setFocusRingType:NSFocusRingTypeNone];
	[self.disclosureButton setState:expanded ? NSOnState : NSOffState];
	[self addSubview:self.disclosureButton];
	
	NSRect imageViewLeftFrame = disclosureFrame;
	imageViewLeftFrame.origin.x = NSMaxX(imageViewLeftFrame) + TL_DISCLOSURE_BAR_SUBVIEW_SPACING;
	imageViewLeftFrame.size.width = NSHeight(imageViewLeftFrame);
	imageViewLeftFrame = NSInsetRect(imageViewLeftFrame, 0.0f, 1.5f);
	self.imageViewLeft = [[[NSImageView alloc] initWithFrame:imageViewLeftFrame] autorelease];
	[self.imageViewLeft setEditable:NO];
	[self.imageViewLeft setAnimates:YES];
	[self.imageViewLeft setImageFrameStyle:NSImageFrameNone];
	[self.imageViewLeft setImageScaling:NSImageScaleProportionallyDown];
	[self.imageViewLeft setAllowsCutCopyPaste:NO];
	[self.imageViewLeft setImageAlignment:NSImageAlignCenter];
	[self addSubview:self.imageViewLeft];
		
	NSRect labelRect = imageViewLeftFrame;
	labelRect.origin.x = NSMaxX(imageViewLeftFrame) + TL_DISCLOSURE_BAR_SUBVIEW_SPACING;
	labelRect.size.width = NSWidth([self frame]) - NSMinX(labelRect);
	self.labelField = [[[NSTextField alloc] initWithFrame:labelRect] autorelease];
	[self.labelField setEditable:NO];
	[self.labelField setBezeled:NO];
	[self.labelField setDrawsBackground:NO];
	[self.labelField setTextColor:[NSColor blackColor]];
	[[self.labelField cell] setBackgroundStyle:NSBackgroundStyleRaised];
	[[self.labelField cell] setControlSize:NSSmallControlSize];
	[[self.labelField cell] setFont:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:[[self.labelField cell] controlSize]]]];
	[[self.labelField cell] setWraps:NO];
	[[self.labelField cell] setLineBreakMode:NSLineBreakByTruncatingTail];
	[self.labelField setAutoresizingMask:NSViewWidthSizable];
	[self addSubview:self.labelField];
		
	return self;
}

- (id)initWithFrame:(NSRect)frame leftImage:(NSImage *)leftImage label:(NSString *)label expanded:(BOOL)expanded;
{
	if (![self initWithFrame:frame expanded:expanded])
		return nil;
	[self setLeftImage:leftImage];
	[self setLabel:label];
	return self;
}

- (NSArray *)keysForCoding;
{
	return [NSArray arrayWithObjects:nil];
}

- (id)initWithCoder:(NSCoder *)coder;
{
	if (![super initWithCoder:coder])
		return nil;
	for (NSString *key in [self keysForCoding])
		[coder encodeObject:[self valueForKey:key] forKey:key];
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder;
{
	for (NSString *key in [self keysForCoding])
		[self setValue:[coder decodeObjectForKey:key] forKey:key];
	[super encodeWithCoder:coder];
}

- (void)dealloc;
{
	[_disclosureButton release];
	[_imageViewLeft release];
	[_accessoryView release];
	[_labelField release];
	[super dealloc];
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldSize;
{
	[super resizeSubviewsWithOldSize:oldSize];
	[self _adjustSubviewsFollowingResize];
}

- (BOOL)hasDisclosureButton;
{
	return ![self.disclosureButton isHidden];
}

- (void)setHasDisclosureButton:(BOOL)flag;
{
	if (self.hasDisclosureButton == flag)
		return;
	[self.disclosureButton setHidden:self.hasDisclosureButton];
	[self _adjustSubviews];
}

- (void)setLeftImage:(NSImage *)image;
{
	[self.imageViewLeft setImage:image];
}

@dynamic accessoryView;
- (NSView *)accessoryView {
	return _accessoryView;
}
- (void)setAccessoryView:(NSView *)accessoryView;
{
	if (_accessoryView == accessoryView)
		return;
	if (_accessoryView) {
		[_accessoryView removeFromSuperview];
		[_accessoryView release];
	}
	_accessoryView = [accessoryView retain];
	NSRect accessoryViewFrame = [_accessoryView frame];
	accessoryViewFrame.origin.x = NSMaxX([self frame]) - NSWidth(accessoryViewFrame);
	[accessoryView setFrame:accessoryViewFrame];
	
	if (([accessoryView autoresizingMask] & NSViewMinXMargin) == 0)
		[accessoryView setAutoresizingMask:NSViewMinXMargin];
	
	[self addSubview:_accessoryView];
	
	NSRect labelFieldFrame = [self.labelField frame];
	labelFieldFrame.size.width = NSWidth([self frame]) - NSMinX(labelFieldFrame) - NSWidth(accessoryViewFrame);
	[self.labelField setFrame:labelFieldFrame];
}

- (void)setLabel:(NSString *)label;
{
	NSSize size = [label sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:[[self.labelField cell] controlSize]]],NSFontAttributeName,nil]];
	NSRect frame = [self.labelField frame];
	frame.size.height = size.height;
	frame.origin.y = NSMidY([self frame]) - size.height / 2.0;
	[self.labelField setFrame:frame];
	[self.labelField setStringValue:label];
}

- (NSString *)label;
{
	return [self.labelField stringValue];
}

- (void)mouseDown:(NSEvent *)event;
{
	self.fillOption = TLGradientViewClickedGradient;
}

- (void)mouseUp:(NSEvent *)event;
{
	self.fillOption = TLGradientViewActiveGradient;
	
	NSPoint mouseLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	if (NSMouseInRect(mouseLocation, [self bounds], [self isFlipped]))
		[self.disclosureButton sendAction:[self.disclosureButton action] to:[self.disclosureButton target]];
}

- (void)mouseDragged:(NSEvent *)event;
{
	NSPoint mouseLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	if (NSMouseInRect(mouseLocation, [self bounds], [self isFlipped]))
		self.fillOption = TLGradientViewClickedGradient;
	else
		self.fillOption = TLGradientViewActiveGradient;
}

@end
