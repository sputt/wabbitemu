//
//  RSCalculatorWindow.m
//  WabbitStudio
//
//  Created by William Towe on 5/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSCalculatorWindow.h"


@implementation RSCalculatorWindow

- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)anItem {
	if ([anItem action] == @selector(performClose:))
		return YES;
	return [super validateUserInterfaceItem:anItem];
}

- (void)performClose:(id)sender {
	if ([self styleMask] == NSBorderlessWindowMask)
		[self close];
	else
		[super performClose:nil];
}

- (BOOL)canBecomeKeyWindow {
	return YES;
}

@end
