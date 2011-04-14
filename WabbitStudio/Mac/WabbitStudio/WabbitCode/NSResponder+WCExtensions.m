//
//  NSResponder+WCExtensions.m
//  WabbitCode
//
//  Created by William Towe on 1/7/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import "NSResponder+WCExtensions.h"


@implementation NSResponder (WCExtensions)
- (IBAction)approveModalWindowAction:(id)sender; {
	[[NSApp modalWindow] close];
	[NSApp stopModalWithCode:NSOKButton];
}

- (IBAction)dismissModalWindowAction:(id)sender; {
	[[NSApp modalWindow] close];
	[NSApp stopModalWithCode:NSCancelButton];
}

- (IBAction)approveModalSheetAction:(id)sender; {
	NSWindow *sheet = [NSApp mainWindow];
	while ([sheet attachedSheet])
		sheet = [sheet attachedSheet];
	[sheet close];
	[NSApp endSheet:sheet returnCode:NSOKButton];
}

- (IBAction)dismissModalSheetAction:(id)sender; {
	NSWindow *sheet = [NSApp mainWindow];
	while ([sheet attachedSheet])
		sheet = [sheet attachedSheet];
	[sheet close];
	[NSApp endSheet:sheet returnCode:NSCancelButton];
}
@end
