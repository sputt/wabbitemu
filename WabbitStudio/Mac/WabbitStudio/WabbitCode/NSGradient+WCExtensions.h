//
//  NSGradient+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 4/15/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSGradient.h>


@interface NSGradient (NSGradient_WCExtensions)
+ (NSGradient *)aquaSelectedGradient;
+ (NSGradient *)aquaNormalGradient;
+ (NSGradient *)aquaPressedGradient;

+ (NSGradient *)unifiedSelectedGradient;
+ (NSGradient *)unifiedNormalGradient;
+ (NSGradient *)unifiedPressedGradient;
+ (NSGradient *)unifiedDarkGradient;
@end
