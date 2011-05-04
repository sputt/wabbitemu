//
//  WEPreferencesController.m
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreferencesController.h"
#import "WELCDView.h"
#import "RSCalculatorSkinView.h"
#include "lcd.h"


NSString *const kWEPreferencesDisplayLCDShadesKey = @"LCDShades";
NSString *const kWEPreferencesDisplayLCDModeKey = @"LCDMode";

NSString *const kWEPreferencesGeneralOnStartupKey = @"generalOnStartup";

@implementation WEPreferencesController

+ (void)initialize {
	NSMutableDictionary *preferences = [NSMutableDictionary dictionary];
	
	// on startup
	[preferences setObject:[NSNumber numberWithUnsignedInteger:WEGeneralOnStartupShowOpenPanel] forKey:kWEPreferencesGeneralOnStartupKey];
	
	// use LCD wire pattern
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kLCDUseWirePatternKey];
	// default LCD shades; lcd->shades
	[preferences setObject:[NSNumber numberWithUnsignedInteger:4] forKey:kWEPreferencesDisplayLCDShadesKey];
	// default LCD Mode; lcd->mode
	[preferences setObject:[NSNumber numberWithUnsignedInteger:MODE_PERFECT_GRAY] forKey:kWEPreferencesDisplayLCDModeKey];
	
	// appearance skins
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kRSCalculatorSkinViewUseSkinsKey];
	// appearance borderless skins
	[preferences setObject:[NSNumber numberWithBool:NO] forKey:kRSCalculatorSkinViewUseBorderlessSkinsKey];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:preferences];
}

@end
