//
//  NSPopUpButton+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSPopUpButton.h>


@interface NSPopUpButton (NSPopUpButton_WCExtensions)
- (void)selectItemWithRepresentedObject:(id)object;
- (void)removeItemWithRepresentedObject:(id)object;
@end
