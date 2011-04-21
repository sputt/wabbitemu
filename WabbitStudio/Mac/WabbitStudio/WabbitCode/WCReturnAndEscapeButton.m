//
//  WCReturnAndEscapeButton.m
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCReturnAndEscapeButton.h"


@implementation WCReturnAndEscapeButton

- (BOOL)performKeyEquivalent:(NSEvent *)event {
	if ([[event charactersIgnoringModifiers] isEqualToString:@"\r"] ||
		[event keyCode] == 53) {
		[[self cell] setKeyEquivalent:[event charactersIgnoringModifiers]];
	}
	return [super performKeyEquivalent:event];
}

@end
