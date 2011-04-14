//
//  WCIncludeDirectoriesArrayController.m
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCIncludeDirectoriesArrayController.h"
#import "WCIncludeDirectory.h"
#import "NSURL+WCExtensions.h"

@implementation WCIncludeDirectoriesArrayController

- (IBAction)add:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setPrompt:NSLocalizedString(@"Add", @"Add")];
	[panel setAllowsMultipleSelection:YES];
	[panel setCanChooseDirectories:YES];
	[panel setCanChooseFiles:NO];
	[panel setDelegate:self];
	
	[panel beginSheetModalForWindow:[[NSApp mainWindow] attachedSheet] completionHandler:^(NSInteger result) {
		if (result == NSFileHandlingPanelCancelButton)
			return;
		
		NSArray *existingURLs = [[self arrangedObjects] valueForKeyPath:@"URL"];

		for (NSURL *url in [panel URLs]) {
			BOOL canAdd = YES;
			for (NSURL *eurl in existingURLs) {
				if ([url isEqualToFileURL:eurl]) {
					canAdd = NO;
					break;
				}
			}
			
			if (canAdd)
				[self insertObject:[WCIncludeDirectory includeDirectoryWithURL:url] atArrangedObjectIndex:[[self arrangedObjects] count]];
		}
		
		
	}];
}

@end
