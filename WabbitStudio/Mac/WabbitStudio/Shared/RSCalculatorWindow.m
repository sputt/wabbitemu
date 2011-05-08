//
//  RSCalculatorWindow.m
//  WabbitStudio
//
//  Created by William Towe on 5/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSCalculatorWindow.h"
#import "RSCalculatorSkinView.h"


@implementation RSCalculatorWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag {
	if (!(self = [super initWithContentRect:contentRect styleMask:([[NSUserDefaults standardUserDefaults] boolForKey:kRSCalculatorSkinViewUseSkinsKey] && [[NSUserDefaults standardUserDefaults] boolForKey:kRSCalculatorSkinViewUseBorderlessSkinsKey])?NSBorderlessWindowMask:aStyle backing:bufferingType defer:flag]))
		return nil;
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kRSCalculatorSkinViewUseSkinsKey] && [[NSUserDefaults standardUserDefaults] boolForKey:kRSCalculatorSkinViewUseBorderlessSkinsKey]) {
		[self setBackgroundColor:[NSColor clearColor]];
		[self setOpaque:NO];
		[self setHasShadow:NO];
	}
	
	return self;
}

- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)anItem {
	if ([anItem action] == @selector(performClose:) && [self styleMask] == NSBorderlessWindowMask)
		return YES;
	else if ([anItem action] == @selector(performMiniaturize:) && [self styleMask] == NSBorderlessWindowMask)
		return YES;
	return [super validateUserInterfaceItem:anItem];
}

- (void)performClose:(id)sender {
	// we want to still close from the menu command if we are borderless
	if ([self styleMask] == NSBorderlessWindowMask)
		[self close];
	else
		[super performClose:nil];
}

- (void)performMiniaturize:(id)sender {
	// we want to still miniaturize from the menu command if we are borderless
	if ([self styleMask] == NSBorderlessWindowMask)
		[self miniaturize:nil];
	else
		[super performMiniaturize:nil];
}

// must override this so our borderless window can still become key
- (BOOL)canBecomeKeyWindow {
	return ([self styleMask] == NSBorderlessWindowMask)?YES:[super canBecomeKeyWindow];
}

- (BOOL)canBecomeMainWindow {
	return ([self styleMask] == NSBorderlessWindowMask)?YES:[super canBecomeMainWindow];
}

@end
