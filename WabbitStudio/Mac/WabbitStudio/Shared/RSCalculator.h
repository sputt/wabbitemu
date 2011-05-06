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

extern NSString *const kRSCalculatorModelDidChangeNotification;

@interface RSCalculator : NSObject {
@private
    LPCALC _calc;
	BOOL _isBusy;
	__weak id <RSCalculatorOwner> _owner;
	SEL _breakpointSelector;
}
@property (readonly,nonatomic) LPCALC calc;
@property (assign,nonatomic) BOOL isActive;
@property (assign,nonatomic) BOOL isRunning;
@property (assign,nonatomic) BOOL isBusy;
@property (readonly,nonatomic) id <RSCalculatorOwner> owner;
@property (readonly,nonatomic) SEL breakpointSelector;
@property (readonly,nonatomic) RSCalculatorModel model;
@property (readonly,nonatomic) NSImage *skinImage;
@property (readonly,nonatomic) NSImage *keymapImage;

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

@property (assign,nonatomic) BOOL CPUHalt;
@property (assign,nonatomic) uint8_t CPUBus;
@property (assign,nonatomic) uint32_t CPUFrequency;

@property (assign,nonatomic) BOOL memoryMapBank0RamOrFlash;
@property (assign,nonatomic) BOOL memoryMapBank0Readonly;
@property (assign,nonatomic) uint8_t memoryMapBank0Page;
@property (assign,nonatomic) BOOL memoryMapBank1RamOrFlash;
@property (assign,nonatomic) BOOL memoryMapBank1Readonly;
@property (assign,nonatomic) uint8_t memoryMapBank1Page;
@property (assign,nonatomic) BOOL memoryMapBank2RamOrFlash;
@property (assign,nonatomic) BOOL memoryMapBank2Readonly;
@property (assign,nonatomic) uint8_t memoryMapBank2Page;
@property (assign,nonatomic) BOOL memoryMapBank3RamOrFlash;
@property (assign,nonatomic) BOOL memoryMapBank3Readonly;
@property (assign,nonatomic) uint8_t memoryMapBank3Page;

@property (assign,nonatomic) BOOL interruptsIFF1;
@property (readonly,nonatomic) double interruptsNextTimer1;
@property (readonly,nonatomic) double interruptsTimer1Duration;
@property (assign,nonatomic) BOOL interruptsIFF2;
@property (readonly,nonatomic) double interruptsNextTimer2;
@property (readonly,nonatomic) double interruptsTimer2Duration;

@property (assign,nonatomic) BOOL displayActive;
@property (assign,nonatomic) uint32_t displayContrast;
@property (assign,nonatomic) int32_t displayX;
@property (assign,nonatomic) int32_t displayY;
@property (assign,nonatomic) int32_t displayZ;
@property (assign,nonatomic) LCD_CURSOR_MODE displayCursorMode;

- (void)step;
- (void)stepOver;
- (void)stepOut;

- (void)simulateKeyPress:(uint16_t)keyCode;
- (void)simulateKeyPress:(uint16_t)keyCode lastKeyPressInSeries:(BOOL)lastKeyPressInSeries;

- (BOOL)loadRomOrSavestate:(NSString *)romOrSavestatePath error:(NSError **)outError;

- (void)setBreakpointOfType:(RSBreakpointType)breakpointType atAddress:(uint16_t)address onPage:(uint8_t)page;

+ (RSCalculator *)calculatorWithOwner:(id <RSCalculatorOwner>)owner breakpointSelector:(SEL)breakpointSelector;
- (id)initWithOwner:(id <RSCalculatorOwner>)owner breakpointSelector:(SEL)breakpointSelector;
@end
