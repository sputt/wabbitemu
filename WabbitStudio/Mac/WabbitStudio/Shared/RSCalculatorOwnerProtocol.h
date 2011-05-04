//
//  RSCalculatorOwnerProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@class RSCalculator;

@protocol RSCalculatorOwner <NSObject>
@optional
@property (readonly,nonatomic) RSCalculator *calculator;
@property (readonly,nonatomic) NSWindow *calculatorWindow;
@property (assign,nonatomic) BOOL isDebugging;
@property (readonly,nonatomic) BOOL isBorderlessSkin;

- (void)updateFPSString;
- (BOOL)presentError:(NSError *)error;
- (NSWindow *)windowForSheet;
@end
