//
//  RSCalculatorProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#include "calc.h"


@protocol RSCalculatorProtocol <NSObject>
@required
@property (readonly,nonatomic) LPCALC calc;
@property (assign,nonatomic) BOOL isRunning;
@property (assign,nonatomic) BOOL isActive;
@property (readonly,nonatomic) NSWindow *calculatorWindow;

@optional
- (void)updateFPSString;
@end
