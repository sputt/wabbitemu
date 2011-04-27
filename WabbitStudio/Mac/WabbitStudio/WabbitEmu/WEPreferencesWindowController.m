//
//  WEPreferencesWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreferencesWindowController.h"


@implementation WEPreferencesWindowController

+ (NSString *)nibName {
	return @"WEPreferencesWindow";
}

- (void)setupToolbar {
	[self addView:_general label:NSLocalizedString(@"General", @"General") image:[NSImage imageNamed:NSImageNamePreferencesGeneral]];
	[self addView:_advanced label:NSLocalizedString(@"Advanced", @"Advanced")];
}

@end
