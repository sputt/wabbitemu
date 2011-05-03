//
//  WEPreferencesWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "DBPrefsWindowController.h"
#import "RSCalculatorOwnerProtocol.h"


@class WEPreviewLCDView,RSCalculator;

@interface WEPreferencesWindowController : DBPrefsWindowController <NSWindowDelegate,RSCalculatorOwner> {
@private
    IBOutlet NSView *_general;
	IBOutlet NSView *_advanced;
	IBOutlet NSView *_calculator;
	
	IBOutlet WEPreviewLCDView *_LCDView;
	
	RSCalculator *_mcalculator;
}

@end
