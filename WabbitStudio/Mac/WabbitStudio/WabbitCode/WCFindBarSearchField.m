//
//  WCFindBarSearchField.m
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindBarSearchField.h"
#import "WCFindBarViewController.h"
#import "WCTextView.h"


@implementation WCFindBarSearchField
- (BOOL)performKeyEquivalent:(NSEvent *)event {
	if ([[event charactersIgnoringModifiers] isEqualToString:@"g"]) {
		[_findBarViewController findNext:nil];
		return YES;
	}
	else if ([[event charactersIgnoringModifiers] isEqualToString:@"G"]) {
		[_findBarViewController findPrevious:nil];
		return YES;
	}
	return [super performKeyEquivalent:event];
}
@end
