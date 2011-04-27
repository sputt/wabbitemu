//
//  WECalculator.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSDocument.h>
#include "calc.h"

@class WELCDView;

@interface WECalculator : NSDocument <NSWindowDelegate> {
@private
	IBOutlet WELCDView *_LCDView;
	
	LPCALC _calc;
}
@property (readonly,nonatomic) LPCALC calc;
@property (readonly,nonatomic) WELCDView *LCDView;
@property (assign,nonatomic) BOOL isRunning;

- (IBAction)loadRom:(id)sender;

- (IBAction)saveStateAs:(id)sender;

- (BOOL)loadRom:(NSURL *)romURL error:(NSError **)outError;
@end
