//
//  WCFileWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/19/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFileWindowController.h"
#import "WCProject.h"
#import "WCFile.h"
#import "WCTextView.h"

#import <PSMTabBarControl/PSMTabBarControl.h>
#import <BWToolkitFramework/BWAnchoredButtonBar.h>


@implementation WCFileWindowController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_file = nil;
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WCFileWindow";
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
	NSDictionary *fDict = [[[[self project] projectSettings] objectForKey:kWCProjectSettingsFileSettingsDictionaryKey] objectForKey:[[self file] UUID]];
	NSString *frameString = [fDict objectForKey:kWCProjectSettingsFileSettingsFileSeparateEditorWindowFrameKey];
	
	if (frameString != nil)
		[[self window] setFrameFromString:frameString];
	
	[_tabBarControl setHideForSingleTab:NO];
	[_tabBarControl setCanCloseOnlyTab:NO];
	[_tabBarControl setStyleNamed:@"Unified"];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
	
	[[self project] addFileViewControllerForFile:[self file] inTabViewContext:self];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName {
	return [NSString stringWithFormat:NSLocalizedString(@"%@ - %@", @"file window title"),displayName,[[self file] name]];
}

- (NSWindow *)tabWindow {
	return [self window];
}
- (PSMTabBarControl *)tabBarControl {
	return _tabBarControl;
}
- (WCTextView *)selectedTextView {
	WCTextView *retval = nil;
	for (NSView *view in [[[[[self tabBarControl] tabView] selectedTabViewItem] view] subviews]) {
		if ([view isKindOfClass:[NSScrollView class]])
			retval = (WCTextView *)[(NSScrollView *)view documentView];
	}
#ifdef DEBUG
    NSAssert(retval != nil, @"selectedTextView cannot be nil!");
#endif
	return retval;
}

- (BOOL)windowShouldClose:(id)sender {
	return [[self project] tabView:[[self tabBarControl] tabView] shouldCloseTabViewItem:[[[self tabBarControl] tabView] selectedTabViewItem]];
}

- (void)windowWillClose:(NSNotification *)notification {
	[[self project] removeFileViewControllerForFile:[self file] inTabViewContext:self];
	
	NSMutableDictionary *filesDict = [[[self project] projectSettings] objectForKey:kWCProjectSettingsFileSettingsDictionaryKey];
	
	if (!filesDict) {
		filesDict = [NSMutableDictionary dictionary];
		[[[self project] projectSettings] setObject:filesDict forKey:kWCProjectSettingsFileSettingsDictionaryKey];
	}
	
	NSMutableDictionary *fDict = [filesDict objectForKey:[[self file] UUID]];
	
	if (!fDict) {
		fDict = [NSMutableDictionary dictionary];
		[filesDict setObject:fDict forKey:[[self file] UUID]];
	}
	
	[fDict setObject:[[self window] stringWithSavedFrame] forKey:kWCProjectSettingsFileSettingsFileSeparateEditorWindowFrameKey];
}

+ (id)fileWindowControllerWithFile:(WCFile *)file; {
	return [[[[self class] alloc] initWithFile:file] autorelease];
}

- (id)initWithFile:(WCFile *)file; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_file = file;
	
	return self;
}

@synthesize file=_file;
@dynamic project;
- (WCProject *)project {
	return (WCProject *)[self document];
}
@end
