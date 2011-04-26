//
//  WCBuildMessagesViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildMessagesViewController.h"
#import "WCProject.h"
#import "WCFile.h"
#import "WCBuildMessage.h"
#import "WCAlias.h"
#import "NSTreeController+WCExtensions.h"
#import "WCBadgedTextFieldCell.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCDefines.h"


@implementation WCBuildMessagesViewController
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"WCBuildMessagesView";
}

- (void)loadView {
	[super loadView];
	
	[[self outlineView] setDoubleAction:@selector(buildMessagesOutlineViewDoubleClick:)];
}

- (NSArray *)selectedObjects {
	return [(NSTreeController *)[[self outlineView] dataSource] selectedRepresentedObjects];
}
- (void)setSelectedObjects:(NSArray *)selectedObjects {
	[(NSTreeController *)[[self outlineView] dataSource] setSelectedRepresentedObjects:selectedObjects];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSOutlineViewDelegate
- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item {	
	WCBuildMessage *message = [item representedObject];
	
	[cell setIcon:[message icon]];
	[cell setIconSize:WCSmallSize];
	[cell setBadgeCount:[message messagesCount]];
	
	if ([[item representedObject] parentNode] == nil) {
		[cell setSecondaryTitle:nil];
		[cell setCenterIcon:YES];
	}
	else {
		[cell setSecondaryTitle:[NSString stringWithFormat:NSLocalizedString(@"%@ - line %lu", @"build message secondary title"),[[message file] name],[message lineNumber]+1]];
		[cell setCenterIcon:NO];
	}
}

- (NSString *)outlineView:(NSOutlineView *)outlineView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tc item:(id)item mouseLocation:(NSPoint)mouseLocation {
	WCBuildMessage *message = [item representedObject];
	
	if ([message messageType] == WCBuildMessageTypeFile)
		return [[message file] absolutePathForDisplay];
	return [NSString stringWithFormat:NSLocalizedString(@"%@\n%@ - line %u", @"build message tooltip"),[message name],[[message file] name],[message lineNumber]];
}

- (CGFloat)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item {
	if ([[item representedObject] parentNode] == nil)
		return [outlineView rowHeight];
	return floor([outlineView rowHeight]*1.5);
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldShowCellExpansionForTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}

- (IBAction)buildMessagesOutlineViewSingleClick:(id)sender; {
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithSingleClick)
		return;
	
	[[self project] jumpToObjects:[self selectedObjects]];
}
- (IBAction)buildMessagesOutlineViewDoubleClick:(id)sender; {
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithDoubleClick)
		return;
	
	[[self project] jumpToObjects:[self selectedObjects]];
}

@synthesize outlineView=_outlineView;

@end
