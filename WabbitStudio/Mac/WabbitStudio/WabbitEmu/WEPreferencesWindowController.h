//
//  WEPreferencesWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "DBPrefsWindowController.h"


@class WEPreviewLCDView;

@interface WEPreferencesWindowController : DBPrefsWindowController <NSWindowDelegate> {
@private
    IBOutlet NSView *_general;
	IBOutlet NSView *_advanced;
	IBOutlet NSView *_calculator;
	
	IBOutlet WEPreviewLCDView *_LCDView;
}

@end
