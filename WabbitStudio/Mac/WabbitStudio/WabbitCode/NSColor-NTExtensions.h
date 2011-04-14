//
//  NSColor-NTExtensions.h
//  CocoatechCore
//
//  Created by Steve Gehrman on Thu Aug 22 2002.
//  Copyright (c) 2002 CocoaTech. All rights reserved.
//

#import <Cocoa/Cocoa.h>
//#import <Carbon/Carbon.h>

@interface NSColor (NTExtensions)

- (BOOL)isDarkerThan:(float)lightness;

- (NSColor*)darkerColor;
- (NSColor*)lighterColor;
- (NSColor*)lighterColor:(float)percent;

// converts a color to an image, draws color on white background incase of alpha component
/*
- (NSColor*)imageColor;
- (NSColor*)opaqueColor;  // returns imageColor if color has alpha, otherwise returns self
*/
@end
