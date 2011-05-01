//
//  WCProjectFilesOutlineViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.	
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectFilesOutlineViewController.h"
#import "WCProject.h"
#import "WCBadgedTextFieldCell.h"
#import "WCFile.h"
#import "WCAlias.h"
#import "NSTreeController+WCExtensions.h"
#import "WCBuildTarget.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCDefines.h"


@implementation WCProjectFilesOutlineViewController
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	//[[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	[[self outlineView] setDoubleAction:@selector(filesOutlineViewDoubleClick:)];
}

- (NSString *)viewNibName {
	return @"WCProjectFilesOutlineView";
}

- (NSArray *)selectedNodes {
	return [(NSTreeController *)[[self outlineView] dataSource] selectedNodes];
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
	WCFile *file = [item representedObject];
	
	[cell setIcon:[file icon]];
	[cell setIconSize:WCSmallSize];
	
	if (![file parentNode]) {
		//[cell setSecondaryTitle:[NSString stringWithFormat:NSLocalizedString(@"%lu target(s)", @"project file secondary title"),[[[self project] buildTargets] count]]];
		NSMutableAttributedString *attributedString = [[[cell attributedStringValue] mutableCopy] autorelease];
		[attributedString applyFontTraits:NSBoldFontMask range:NSMakeRange(0, [[attributedString string] length])];
		[cell setAttributedStringValue:attributedString];
	}
	else {
		//[cell setSecondaryTitle:nil];
	}
}

- (NSString *)outlineView:(NSOutlineView *)outlineView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tc item:(id)item mouseLocation:(NSPoint)mouseLocation {
	return [[[item representedObject] alias] absolutePathForDisplay];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return [[item representedObject] canEditName];
}

- (CGFloat)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item {
	//if (![[item representedObject] parentNode])
	//return floor([outlineView rowHeight]*1.5);
	return [outlineView rowHeight];
}
#pragma mark *** Public Methods ***

#pragma mark Accessors
@synthesize outlineView=_outlineView;
#pragma mark IBActions
- (IBAction)filesOutlineViewSingleClick:(id)sender; {
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithSingleClick)
		return;
	
	NSArray *selectedNodes = [self selectedNodes];
	
	for (NSTreeNode *node in selectedNodes) {
		if ([[node representedObject] isTextFile])
			[[self project] addFileViewControllerForFile:[node representedObject] inTabViewContext:[[self project] currentTabViewContext]];
	}
}
- (IBAction)filesOutlineViewDoubleClick:(id)sender; {
	NSArray *selectedNodes = [self selectedNodes];
	
	/*
	if ([selectedNodes count] == 1 &&
		![[[selectedNodes lastObject] representedObject] isLeaf]) {
		
		if ([[self outlineView] isItemExpanded:[selectedNodes lastObject]])
			[[self outlineView] collapseItem:[selectedNodes lastObject]];
		else
			[[self outlineView] expandItem:[selectedNodes lastObject]];
	}
	 */
	
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithDoubleClick)
		return;
	
	for (NSTreeNode *node in selectedNodes) {
		if ([[node representedObject] isTextFile])
			[[self project] addFileViewControllerForFile:[node representedObject] inTabViewContext:[[self project] currentTabViewContext]];
	}
}
#pragma mark *** Private Methods ***
/*
- (void)_updateProjectFileRow:(NSNotification *)note {
	[[self outlineView] setNeedsDisplayInRect:[[self outlineView] rectOfRow:0]];
}
 */
@end
