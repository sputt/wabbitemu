//
//  TLGradientView.m
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

#import "TLGradientView.h"



@interface TLGradientView ()

@end

@interface TLGradientView (Private)

@end

@implementation TLGradientView (Private)

@end

@implementation TLGradientView
@synthesize highlightColor = _highlightColor;
@synthesize clickedHighlightColor = _clickedHighLightColor;

- (id)init;
{
	if (![super init])
		return nil;

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(display) name:NSApplicationDidBecomeActiveNotification object:NSApp];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(display) name:NSApplicationDidResignActiveNotification object:NSApp];
	
	return self;
}

- (id)initWithFrame:(NSRect)frame;
{
	if (![super initWithFrame:frame])
		return nil;
	
	self.activeFillGradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],[NSColor colorWithCalibratedWhite:0.814 alpha:1.0],nil]] autorelease];
	self.inactiveFillGradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],nil]] autorelease];
	self.clickedFillGradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:[NSColor colorWithCalibratedWhite:0.916 alpha:1.0],[NSColor colorWithCalibratedWhite:0.814 alpha:1.0],nil]] autorelease];
	self.fillOption = TLGradientViewActiveGradient;
	self.fillAngle = 270.0f;
	
	self.borderColor = [NSColor lightGrayColor];
	self.borderSidesMask = (TLMinXEdge|TLMaxXEdge|TLMinYEdge|TLMaxYEdge);
	
	self.highlightColor = [NSColor colorWithCalibratedWhite:0.97f alpha:1.0f];
	
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
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_activeFillGradient release];
	[_inactiveFillGradient release];
	[_clickedFillGradient release];
	[_borderColor release];
	[_highlightColor release];
	[super dealloc];
}

- (void)viewWillMoveToSuperview:(NSView *)superview;
{
	[super viewWillMoveToSuperview:superview];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowDidResignKeyNotification object:[self window]];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowDidBecomeKeyNotification object:[self window]];
	
	if (!superview) return;
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(display) name:NSWindowDidResignKeyNotification object:[superview window]];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(display) name:NSWindowDidBecomeKeyNotification object:[superview window]];
}

@dynamic borderSidesMask;
- (TLRectEdge)borderSidesMask {
	return _borderSidesMask;
}
- (void)setBorderSidesMask:(TLRectEdge)mask;
{
	_borderSidesMask = mask;
	[self setNeedsDisplay:YES];
}

@dynamic clickedFillGradient;
- (NSGradient *)clickedFillGradient {
	return _clickedFillGradient;
}
- (void)setClickedFillGradient:(NSGradient *)gradient;
{
	if (_clickedFillGradient == gradient)
		return;
	[gradient retain];
	[_clickedFillGradient release];
	_clickedFillGradient = gradient;
	[self setNeedsDisplay:YES];
}

@dynamic activeFillGradient;
- (NSGradient *)activeFillGradient {
	return _activeFillGradient;
}
- (void)setActiveFillGradient:(NSGradient *)gradient;
{
	if (_activeFillGradient == gradient)
		return;
	[gradient retain];
	[_activeFillGradient release];
	_activeFillGradient = gradient;
	[self setNeedsDisplay:YES];
}

@dynamic inactiveFillGradient;
- (NSGradient *)inactiveFillGradient {
	return _inactiveFillGradient;
}
- (void)setInactiveFillGradient:(NSGradient *)gradient;
{
	if (_inactiveFillGradient == gradient)
		return;
	[gradient retain];	
	[_inactiveFillGradient release];
	_inactiveFillGradient = gradient;
	[self setNeedsDisplay:YES];
}

@dynamic fillOption;
- (TLGradientViewFillOption)fillOption {
	return _fillOption;
}
- (void)setFillOption:(TLGradientViewFillOption)options;
{
	_fillOption = options;
	[self setNeedsDisplay:YES];
}

@dynamic fillAngle;
- (CGFloat)fillAngle {
	return _fillAngle;
}
- (void)setFillAngle:(CGFloat)angle;
{
	_fillAngle = angle;
	[self setNeedsDisplay:YES];
}

@dynamic drawsHighlight;
- (BOOL)drawsHighlight {
	return _drawsHighlight;
}

- (void)setDrawsHighlight:(BOOL)flag;
{
	if (_drawsHighlight == flag)
		return;
	_drawsHighlight = flag;
	[self setNeedsDisplay:YES];
}

@dynamic drawsBorder;
- (BOOL)drawsBorder {
	return _drawsBorder;
}
- (void)setDrawsBorder:(BOOL)flag;
{
	if (_drawsBorder == flag)
		return;
	_drawsBorder = flag;
	[self setNeedsDisplay:YES];
}

@dynamic borderColor;
- (NSColor *)borderColor {
	return _borderColor;
}
- (void)setBorderColor:(NSColor *)color;
{
	if (_borderColor == color)
		return;
	[color retain];	
	[_borderColor release];
	_borderColor = color;
	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect;
{
	NSGradient *fillGradient = nil;
	if (self.fillOption != TLGradientViewClickedGradient)
		fillGradient = [[self window] isKeyWindow] ? self.activeFillGradient : self.inactiveFillGradient;
	else
		fillGradient = self.clickedFillGradient;
	
	[fillGradient drawInRect:[self bounds] angle:self.fillAngle];
	
	if (self.drawsBorder) {
		[self.borderColor setStroke];
		NSBezierPath *border = [NSBezierPath bezierPath];
		NSRect bounds = [self bounds];
		if (self.borderSidesMask & TLMinXEdge)
			[border appendBezierPath:[NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX(bounds) + 0.5f, NSMinY(bounds), 0.0f, NSHeight(bounds))]];
		if (self.borderSidesMask & TLMaxXEdge)
			[border appendBezierPath:[NSBezierPath bezierPathWithRect:NSMakeRect(NSMaxX(bounds) - 0.5f, NSMinY(bounds), 0.0f, NSHeight(bounds))]];
		if (self.borderSidesMask & TLMinYEdge)
			[border appendBezierPath:[NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX(bounds), NSMinY(bounds) + 0.5f, NSWidth(bounds), 0.0f)]];
		if (self.borderSidesMask & TLMaxYEdge)
			[border appendBezierPath:[NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX(bounds), NSMaxY(bounds) - 0.5f, NSWidth(bounds), 0.0f)]];
		[border stroke];
	}
	
	if (self.drawsHighlight) {
		[self.highlightColor setStroke];
		[[NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX([self bounds]), [self isFlipped] ? NSMinY([self bounds]) + (self.borderSidesMask & TLMinYEdge ? 1.5f : 0.5f) : NSMaxY([self bounds]) - (self.borderSidesMask & TLMaxYEdge ? 1.5f : 0.5f), NSWidth([self bounds]), 0.0f)] stroke];
	}
}

@end
