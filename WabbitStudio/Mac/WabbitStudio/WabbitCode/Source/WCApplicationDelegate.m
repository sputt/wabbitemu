//
//  WCApplicationDelegate.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCApplicationDelegate.h"
#import "WCPreferencesController.h"
#import "WCNewProjectWindowController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "NSArray+WCExtensions.h"
#import "WCPreferencesWindowController.h"


@implementation WCApplicationDelegate

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender {
	return NO;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
	WCPreferencesOnStartup action = [[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesOnStartupKey];
	
	switch (action) {
		case WCPreferencesOnStartupShowNewProjectWindow:
			[[WCNewProjectWindowController sharedWindowController] presentWindowController];
			break;
		case WCPreferencesOnStartupOpenMostRecentProject:
			if ([[[NSDocumentController sharedDocumentController] recentDocumentURLs] count])
				[[NSDocumentController sharedDocumentController] openDocumentWithContentsOfURL:[[[NSDocumentController sharedDocumentController] recentDocumentURLs] firstObject] display:YES error:NULL];
			break;
		case WCPreferencesOnStartupDoNothing:
		default:
			break;
	}
}
#pragma mark *** Public Methods ***
#pragma mark IBActions
- (IBAction)newProject:(id)sender; {
	[[WCNewProjectWindowController sharedWindowController] presentWindowController];
}

- (IBAction)preferences:(id)sender; {
	[[WCPreferencesWindowController sharedPrefsWindowController] showWindow:nil];
}
@end
