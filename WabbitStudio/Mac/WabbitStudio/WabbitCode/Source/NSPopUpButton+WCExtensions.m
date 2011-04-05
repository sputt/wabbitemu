//
//  NSPopUpButton+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSPopUpButton+WCExtensions.h"


@implementation NSPopUpButton (NSPopUpButton_WCExtensions)
- (void)selectItemWithRepresentedObject:(id)object; {
	for (NSMenuItem *item in [[self menu] itemArray]) {
		if ([item representedObject] == object) {
			[self selectItem:item];
			return;
		}
	}
}
@end
