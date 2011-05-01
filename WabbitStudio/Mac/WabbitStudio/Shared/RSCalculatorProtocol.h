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
@property (assign,nonatomic) BOOL isLoadingRom;
@property (readonly,nonatomic) NSWindow *calculatorWindow;
@property (assign,nonatomic) BOOL isDebugging;

- (BOOL)loadRomOrSavestate:(NSURL *)fileURL error:(NSError **)outError;
- (BOOL)presentError:(NSError *)error;

- (void)simulateKeyPress:(uint16_t)keyCode;
- (void)simulateKeyPress:(uint16_t)keyCode lastKeyPressInSeries:(BOOL)lastKeyPressInSeries;

- (IBAction)step:(id)sender;
- (IBAction)stepOver:(id)sender;
- (IBAction)stepOut:(id)sender;

@optional
- (void)updateFPSString;
@end
