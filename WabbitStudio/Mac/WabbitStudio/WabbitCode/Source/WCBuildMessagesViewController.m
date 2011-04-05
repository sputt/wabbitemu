//
//  WCBuildMessagesViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildMessagesViewController.h"
#import "WCProject.h"
#import "WCBadgedTextFieldCell.h"
#import "WCFile.h"
#import "WCBuildMessage.h"
#import "WCAlias.h"
#import "NSTreeController+WCExtensions.h"

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
	
	[[self outlineView] setDoubleAction:@selector(_buildMessagesOutlineViewDoubleAction:)];
	[[self outlineView] setTarget:[self project]];
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
	[cell setBadgeCount:[[item representedObject] messagesCount]];
	
	/*
	if (![message parentNode])
		[cell setIconSize:NSMakeSize(24.0, 24.0)];
	else
		[cell setIconSize:NSMakeSize(16.0, 16.0)];
	 */
}

- (NSString *)outlineView:(NSOutlineView *)outlineView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tc item:(id)item mouseLocation:(NSPoint)mouseLocation {
	WCBuildMessage *message = [item representedObject];
	
	if ([message messageType] == WCBuildMessageTypeFile)
		return [[[message file] alias] absolutePathForDisplay];
	return [NSString stringWithFormat:NSLocalizedString(@"%@ - line %u\n%@", @"build message tooltip"),[[message file] name],[message lineNumber],[message name]];
}
/*
- (CGFloat)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item {
	if (![[item representedObject] parentNode])
		return 32.0;
	return [outlineView rowHeight];
}
 */

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldShowCellExpansionForTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}
@synthesize outlineView=_outlineView;
@end
