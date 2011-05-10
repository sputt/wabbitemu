//
//  NSWindow-Zoom.m
//  WindowZoom
//
//  Copyright 2007 Noodlesoft, L.L.C.. All rights reserved.
//
//  This code is provided under the MIT license:
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#import "NSWindow-NoodleEffects.h"
#import <Carbon/Carbon.h>

#define ZOOM_ANIMATION_TIME_MULTIPLIER			0.5
#define SLOW_ZOOM_ANIMATION_TIME_MULTIPLIER		5

@interface NoodleZoomWindow : NSWindow

@end

@implementation NoodleZoomWindow

- (NSTimeInterval)animationResizeTime:(NSRect)newWindowFrame
{
	float			multiplier;
	
	multiplier = ZOOM_ANIMATION_TIME_MULTIPLIER;
	
	if (GetCurrentEventKeyModifiers() & shiftKey)
	{

		multiplier = SLOW_ZOOM_ANIMATION_TIME_MULTIPLIER;
	}
	
	return [super animationResizeTime:newWindowFrame] * multiplier;
}

@end

@implementation NSWindow (NoodleEffects)

- (NSWindow *)_createZoomWindowWithRect:(NSRect)rect
{
    NSWindow        *zoomWindow;
    NSImageView     *imageView;
    NSImage         *image;
    NSRect          frame;
    BOOL            isOneShot;
    
    frame = [self frame];

    isOneShot = [self isOneShot];
	if (isOneShot)
	{
		[self setOneShot:NO];
	}
    
	if ([self windowNumber] <= 0)
	{
        // Force window device. Kinda crufty but I don't see a visible flash
		// when doing this. May be a timing thing wrt the vertical refresh.
        [self orderBack:self];
        [self orderOut:self];
	}
    
    image = [[NSImage alloc] initWithSize:frame.size];
    [image lockFocus];
    // Grab the window's pixels
    NSCopyBits([self gState], NSMakeRect(0.0, 0.0, frame.size.width, frame.size.height), NSZeroPoint);
    [image unlockFocus];
    
    zoomWindow = [[NoodleZoomWindow alloc] initWithContentRect:rect styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:NO];
    [zoomWindow setBackgroundColor:[NSColor colorWithDeviceWhite:0.0 alpha:0.0]];
    [zoomWindow setHasShadow:[self hasShadow]];
	[zoomWindow setLevel:[self level]];
    [zoomWindow setOpaque:NO];
    [zoomWindow setReleasedWhenClosed:YES];
    [zoomWindow useOptimizedDrawing:YES];
    
    imageView = [[NSImageView alloc] initWithFrame:[zoomWindow contentRectForFrameRect:frame]];
    [imageView setImage:image];
    [imageView setImageFrameStyle:NSImageFrameNone];
    [imageView setImageScaling:NSScaleToFit];
    [imageView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
    
    [zoomWindow setContentView:imageView];
    [image release];	
    [imageView release];
    
    // Reset one shot flag
    [self setOneShot:isOneShot];
    
    return zoomWindow;
}

- (void)zoomOnFromRect:(NSRect)startRect
{
    NSRect              frame;
    NSWindow            *zoomWindow;

    if ([self isVisible])
    {
        return;
    }
        
    frame = [self frame];
    
    zoomWindow = [self _createZoomWindowWithRect:startRect];
   
	[zoomWindow orderFront:self];

	[zoomWindow setFrame:frame display:YES animate:YES];
    
	[self makeKeyAndOrderFront:self];	
	[zoomWindow close];
}

- (void)zoomOffToRect:(NSRect)endRect
{
    NSRect              frame;
    NSWindow            *zoomWindow;
    
    frame = [self frame];
    
    if (![self isVisible])
    {
        return;
    }
    
    zoomWindow = [self _createZoomWindowWithRect:frame];
    
	[zoomWindow orderFront:self];
    [self orderOut:self];
    
	[zoomWindow setFrame:endRect display:YES animate:YES];
    
	[zoomWindow close];    
}

@end
