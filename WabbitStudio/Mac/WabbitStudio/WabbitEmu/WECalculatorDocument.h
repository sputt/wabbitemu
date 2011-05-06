//
//  WECalculatorDocument.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSDocument.h>
#import "RSCalculatorOwnerProtocol.h"


extern NSString *const kWECalculatorWillCloseNotification;

@class WELCDView,BWAnchoredButtonBar,RSCalculator,RSCalculatorSkinView,WEDebuggerWindowController;

@interface WECalculatorDocument : NSDocument <NSWindowDelegate,RSCalculatorOwner> {
@private
	IBOutlet WELCDView *_LCDView;
	IBOutlet BWAnchoredButtonBar *_buttonBar;
	IBOutlet NSTextField *_statusTextField;
	
	RSCalculator *_calculator;
	
	NSString *_statusString;
	NSString *_FPSString;
	BOOL _hasSkin;
	BOOL _isBorderlessSkin;
	
	BOOL _isClosing;
	BOOL _isDebugging;
}
@property (readonly,nonatomic) RSCalculator *calculator;
@property (readonly,nonatomic) WELCDView *LCDView;
@property (copy,nonatomic) NSString *statusString;
@property (copy,nonatomic) NSString *FPSString;
@property (readonly,nonatomic) NSWindow *calculatorWindow;
@property (assign,nonatomic) BOOL isDebugging;
@property (assign,nonatomic) BOOL hasSkin;
@property (assign,nonatomic) BOOL isBorderlessSkin;
@property (readonly,nonatomic) NSImage *statusImage;
@property (readonly,nonatomic) WEDebuggerWindowController *debuggerWindowController;

- (IBAction)loadRom:(id)sender;

- (IBAction)saveStateAs:(id)sender;
- (IBAction)reloadCurrentRomOrSavestate:(id)sender;
- (IBAction)resetCalculator:(id)sender;

- (IBAction)toggleLCDSize:(id)sender;

- (IBAction)toggleSkinView:(id)sender;

- (IBAction)toggleEmulation:(id)sender;

- (IBAction)showDebugger:(id)sender;

- (void)updateFPSString;
- (void)updateStatusString;
@end
