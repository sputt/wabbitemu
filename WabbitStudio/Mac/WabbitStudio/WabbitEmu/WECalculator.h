//
//  WECalculator.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSDocument.h>
#import "RSCalculatorProtocol.h"


enum {
	WECalculatorModelTI73 = TI_73,
	WECalculatorModelTI81 = TI_81,
	WECalculatorModelTI82 = TI_82,
	WECalculatorModelTI83 = TI_83,
	WECalculatorModelTI83P = TI_83P,
	WECalculatorModelTI83PSE = TI_83PSE,
	WECalculatorModelTI84P = TI_84P,
	WECalculatorModelTI84PSE = TI_84PSE,
	WECalculatorModelTI85 = TI_85,
	WECalculatorModelTI86 = TI_86
};
typedef NSUInteger WECalculatorModel;

extern NSString *const kWECalculatorProgramUTI;
extern NSString *const kWECalculatorApplicationUTI;
extern NSString *const kWECalculatorSavestateUTI;
extern NSString *const kWECalculatorRomUTI;

extern NSString *const kWECalculatorWillCloseNotification;

@class WELCDView,BWAnchoredButtonBar;

@interface WECalculator : NSDocument <NSWindowDelegate,RSCalculatorProtocol> {
@private
	IBOutlet WELCDView *_LCDView;
	IBOutlet BWAnchoredButtonBar *_buttonBar;
	IBOutlet NSTextField *_statusTextField;
	
	LPCALC _calc;
	
	BOOL _isLoadingRom;
	NSString *_statusString;
	NSString *_FPSString;
	
	BOOL _isClosing;
	BOOL _isDebugging;
}
@property (readonly,nonatomic) LPCALC calc;
@property (readonly,nonatomic) WELCDView *LCDView;
@property (assign,nonatomic) BOOL isRunning;
@property (assign,nonatomic) BOOL isActive;
@property (assign,nonatomic) BOOL isLoadingRom;
@property (copy,nonatomic) NSString *statusString;
@property (copy,nonatomic) NSString *FPSString;
@property (readonly,nonatomic) WECalculatorModel model;
@property (readonly,nonatomic) NSWindow *calculatorWindow;
@property (assign,nonatomic) BOOL isDebugging;

- (IBAction)loadRom:(id)sender;

- (IBAction)saveStateAs:(id)sender;
- (IBAction)reloadCurrentRomOrSavestate:(id)sender;
- (IBAction)resetCalculator:(id)sender;

- (IBAction)toggleLCDSize:(id)sender;
- (IBAction)resetDisplaySize:(id)sender;

- (BOOL)loadRomOrSavestate:(NSURL *)romURL error:(NSError **)outError;

- (void)updateFPSString;
- (void)updateStatusString;
@end
