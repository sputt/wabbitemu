//
//  NSColor-NTExtensions.m
//  CocoatechCore
//
//  Created by Steve Gehrman on Thu Aug 22 2002.
//  Copyright (c) 2002 CocoaTech. All rights reserved.
//

#import "NSColor-NTExtensions.h"
//#import "NTImageMaker.h"

@implementation NSColor (NTExtensions)

- (BOOL)isDarkerThan:(float)lightness
{
    NSColor *monoColor = [self colorUsingColorSpaceName:@"NSCalibratedWhiteColorSpace"];
    
    return ([monoColor whiteComponent] < lightness);
}

- (NSColor*)darkerColor;
{
    CGFloat hue, saturation, brightness, alpha;
    NSColor* converted = [self colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
    
    [converted getHue:&hue saturation:&saturation brightness:&brightness alpha:&alpha];
    
    brightness = MAX(0.0, brightness - (brightness / 4));
	
    return [NSColor colorWithCalibratedHue:hue saturation:saturation brightness:brightness alpha:alpha];
}

- (NSColor*)lighterColor;
{
    CGFloat hue, saturation, brightness, alpha;
    NSColor* converted = [self colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
    
    [converted getHue:&hue saturation:&saturation brightness:&brightness alpha:&alpha];
    
    saturation = MAX(0.0, saturation - (saturation / 4));
	
    return [NSColor colorWithCalibratedHue:hue saturation:saturation brightness:brightness alpha:alpha];
}

- (NSColor*)lighterColor:(float)percent;
{
    CGFloat hue, saturation, brightness, alpha;
    NSColor* converted = [self colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
    
    [converted getHue:&hue saturation:&saturation brightness:&brightness alpha:&alpha];
    	
	if (hue == 0.0 && saturation == 0.0 && brightness != 0)  // gray color
		brightness = MIN(1.0, MAX(0.0, brightness + (brightness * (1.0-percent))));
	else if (saturation != 0.0)
		saturation = MAX(0.0, saturation * percent);
	else if (brightness != 0)
		brightness = MAX(0.0, brightness * percent);
	else
		brightness = MAX(0.0,1 - percent);
	
    return [NSColor colorWithCalibratedHue:hue saturation:saturation brightness:brightness alpha:alpha];
}
/*
- (NSColor*)opaqueColor;
{
	if ([self alphaComponent] != 1.0)
		return [self imageColor];
	
	return self;
}

// converts a color to an image, draws color on white background incase of alpha component
- (NSColor*)imageColor;
{
	NSRect imageRect = NSMakeRect(0,0,128,128);
	NTImageMaker* imageMaker = [NTImageMaker maker:imageRect.size];
	
	[imageMaker lockFocus];
		
	// draw white so colors with alpha components look OK
	[[NSColor whiteColor] set];
	[NSBezierPath fillRect:imageRect];
	
	[self set];
	[NSBezierPath fillRect:imageRect];
	
	return [NSColor colorWithPatternImage:[imageMaker unlockFocus]];
}
*/
@end
