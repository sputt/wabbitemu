//
//  WEPreferencesController.m
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreferencesController.h"
#include "lcd.h"


NSString *const kWEPreferencesDisplayUseLCDWirePatternKey = @"useLCDWirePattern";
NSString *const kWEPreferencesDisplayLCDShadesKey = @"LCDShades";
NSString *const kWEPreferencesDisplayLCDModeKey = @"LCDMode";

@implementation WEPreferencesController

+ (void)initialize {
	NSMutableDictionary *preferences = [NSMutableDictionary dictionary];
	
	// use LCD wire pattern
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWEPreferencesDisplayUseLCDWirePatternKey];
	// default LCD shades; lcd->shades
	[preferences setObject:[NSNumber numberWithUnsignedInteger:4] forKey:kWEPreferencesDisplayLCDShadesKey];
	// default LCD Mode; lcd->mode
	[preferences setObject:[NSNumber numberWithUnsignedInteger:MODE_PERFECT_GRAY] forKey:kWEPreferencesDisplayLCDModeKey];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:preferences];
}

@end
