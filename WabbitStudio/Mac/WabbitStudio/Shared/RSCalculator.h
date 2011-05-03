//
//  RSCalculator.h
//  WabbitStudio
//
//  Created by William Towe on 5/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#import "RSCalculatorOwnerProtocol.h"
#include "calc.h"


enum {
	RSCalculatorModelTI73 = TI_73,
	RSCalculatorModelTI81 = TI_81,
	RSCalculatorModelTI82 = TI_82,
	RSCalculatorModelTI83 = TI_83,
	RSCalculatorModelTI83P = TI_83P,
	RSCalculatorModelTI83PSE = TI_83PSE,
	RSCalculatorModelTI84P = TI_84P,
	RSCalculatorModelTI84PSE = TI_84PSE,
	RSCalculatorModelTI85 = TI_85,
	RSCalculatorModelTI86 = TI_86
};
typedef NSUInteger RSCalculatorModel;

enum {
	RSBreakpointTypeRam = 0,
	RSBreakpointTypeFlash
};
typedef NSUInteger RSBreakpointType;

@interface RSCalculator : NSObject {
@private
    LPCALC _calc;
	BOOL _isBusy;
	__weak NSDocument <RSCalculatorOwner> *_owner;
	SEL _breakpointSelector;
}
@property (readonly,nonatomic) LPCALC calc;
@property (assign,nonatomic) BOOL isActive;
@property (assign,nonatomic) BOOL isRunning;
@property (assign,nonatomic) BOOL isBusy;
@property (readonly,nonatomic) NSDocument <RSCalculatorOwner> *owner;
@property (readonly,nonatomic) SEL breakpointSelector;
@property (readonly,nonatomic) RSCalculatorModel model;

@property (assign,nonatomic) uint16_t programCounter;
@property (assign,nonatomic) uint16_t stackPointer;
@property (assign,nonatomic) uint16_t registerAF;
@property (assign,nonatomic) uint16_t registerAFPrime;
@property (assign,nonatomic) uint16_t registerBC;
@property (assign,nonatomic) uint16_t registerBCPrime;
@property (assign,nonatomic) uint16_t registerDE;
@property (assign,nonatomic) uint16_t registerDEPrime;
@property (assign,nonatomic) uint16_t registerHL;
@property (assign,nonatomic) uint16_t registerHLPrime;
@property (assign,nonatomic) uint16_t registerIX;
@property (assign,nonatomic) uint16_t registerIY;

@property (assign,nonatomic) BOOL zFlag;
@property (assign,nonatomic) BOOL cFlag;
@property (assign,nonatomic) BOOL sFlag;
@property (assign,nonatomic) BOOL pvFlag;
@property (assign,nonatomic) BOOL hcFlag;
@property (assign,nonatomic) BOOL nFlag;

- (void)step;
- (void)stepOver;
- (void)stepOut;

- (void)simulateKeyPress:(uint16_t)keyCode;
- (void)simulateKeyPress:(uint16_t)keyCode lastKeyPressInSeries:(BOOL)lastKeyPressInSeries;

- (BOOL)loadRomOrSavestate:(NSString *)romOrSavestatePath error:(NSError **)outError;

- (void)setBreakpointOfType:(RSBreakpointType)breakpointType atAddress:(uint16_t)address onPage:(uint8_t)page;

+ (RSCalculator *)calculatorWithOwner:(NSDocument <RSCalculatorOwner> *)owner breakpointSelector:(SEL)breakpointSelector;
- (id)initWithOwner:(NSDocument <RSCalculatorOwner> *)owner breakpointSelector:(SEL)breakpointSelector;
@end
