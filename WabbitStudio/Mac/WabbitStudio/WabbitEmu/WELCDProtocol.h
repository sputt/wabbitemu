//
//  WELCDProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#include "calc.h"


#define kLCDWidth 96
#define kLCDWidescreenWidth 128
#define kLCDHeight 64

@class WECalculator;

@protocol WELCDProtocol <NSObject>
@required
- (LPCALC)calc;
- (void)setCalc:(LPCALC)calc;
- (BOOL)isWidescreen;
- (void)setIsWidescreen:(BOOL)isWidescreen;
- (WECalculator *)calculator;
- (void)setNeedsDisplay:(BOOL)flag;
@end
