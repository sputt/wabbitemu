//
//  NSGradient+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 4/15/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSGradient+WCExtensions.h"


@implementation NSGradient (NSGradient_WCExtensions)
+ (NSGradient *)aquaSelectedGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.58 green:0.86 blue:0.98 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.42 green:0.68 blue:0.9 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.64 green:0.8 blue:0.94 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.56 green:0.7 blue:0.9 alpha:1.0],1.0, nil] autorelease];
}
+ (NSGradient *)aquaNormalGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.95 green:0.95 blue:0.95 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.83 green:0.83 blue:0.83 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.95 green:0.95 blue:0.95 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.92 green:0.92 blue:0.92 alpha:1.0],1.0, nil] autorelease];
}
+ (NSGradient *)aquaPressedGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.64 green:0.64 blue:0.64 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.77 green:0.77 blue:0.77 alpha:1.0],1.0, nil] autorelease];
}

+ (NSGradient *)unifiedSelectedGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:0.85 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:0.95 alpha:1.0],1.0, nil] autorelease];
}					
+ (NSGradient *)unifiedNormalGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:0.75 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:0.9 alpha:1.0],1.0, nil] autorelease];
}
+ (NSGradient *)unifiedPressedGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:0.6 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:0.75 alpha:1.0],1.0, nil] autorelease];
}
+ (NSGradient *)unifiedDarkGradient; {
	return [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:0.68 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:0.83 alpha:1.0],1.0, nil] autorelease];
}
@end
