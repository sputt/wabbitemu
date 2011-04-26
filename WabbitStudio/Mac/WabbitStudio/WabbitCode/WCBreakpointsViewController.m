//
//  WCBreakpointsViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/19/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBreakpointsViewController.h"
#import "WCBadgedTextFieldCell.h"
#import "WCBreakpoint.h"
#import "WCProject.h"
#import "WCProjectFile.h"
#import "NSTreeController+WCExtensions.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCDefines.h"


@implementation WCBreakpointsViewController

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_outlineView setDataSource:nil];
	[_outlineView setDelegate:nil];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"WCBreakpointsView";
}

- (void)loadView {
	[super loadView];
	
	[[self outlineView] setDoubleAction:@selector(breakpointsOutlineViewDoubleClick:)];
	
	[[self outlineView] expandItem:nil expandChildren:YES];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileDidAddBreakpoint:) name:kWCFileDidAddBreakpointNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileDidRemoveBreakpoint:) name:kWCFileDidRemoveBreakpointNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_breakpointIsActiveDidChange:) name:kWCBreakpointIsActiveDidChangeNotification object:nil];
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
	
	[cell setIcon:[[item representedObject] icon]];
	if ([[item representedObject] breakpointType] == WCBreakpointTypeLine)
		[cell setIconSize:[[[item representedObject] icon] size]];
	else
		[cell setIconSize:WCSmallSize];
	
	if ([[item representedObject] breakpointType] == WCBreakpointTypeProject ||
		[[item representedObject] breakpointType] == WCBreakpointTypeLine)
		[cell setBadgeCount:0];
	else
		[cell setBadgeCount:[[[item representedObject] childNodes] count]];
	
	if ([[item representedObject] breakpointType] == WCBreakpointTypeLine) {
		[cell setSecondaryTitle:[[item representedObject] symbolNameAndLineNumber]];
	}
	else
		[cell setSecondaryTitle:nil];
	 
	if ([[item representedObject] breakpointType] == WCBreakpointTypeProject) {
		NSMutableAttributedString *attributedString = [[[cell attributedStringValue] mutableCopy] autorelease];
		[attributedString applyFontTraits:NSBoldFontMask range:NSMakeRange(0, [[attributedString string] length])];
		[cell setAttributedStringValue:attributedString];
	}
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	if ([[item representedObject] breakpointType] == WCBreakpointTypeLine)
		return YES;
	return NO;
}

- (CGFloat)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item {
	if ([[item representedObject] breakpointType] == WCBreakpointTypeLine)
		return floor([outlineView rowHeight]*1.5);
	return [outlineView rowHeight];
}

@synthesize outlineView=_outlineView;

- (IBAction)breakpointsOutlineViewSingleClick:(id)sender; {
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithSingleClick)
		return;
	
	[[self project] jumpToObjects:[self selectedObjects]];
}
- (IBAction)breakpointsOutlineViewDoubleClick:(id)sender; {
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithDoubleClick)
		return;
	
	[[self project] jumpToObjects:[self selectedObjects]];
}

- (void)_fileDidAddBreakpoint:(NSNotification *)note {
	WCFile *file = [note object];
	
	if ([file project] != [self project])
		return;
	
	WCBreakpoint *newBreakpoint = [[note userInfo] objectForKey:kWCFileBreakpointKey];
	WCBreakpoint *parentBreakpoint = nil;
	
	// find the parent breakpoint that this new breakpoint should be added to
	for (WCBreakpoint *breakpoint in [[[self project] projectBreakpoint] childNodes]) {
		if ([breakpoint file] == file) {
			parentBreakpoint = breakpoint;
			break;
		}
	}
	
	// the parent breakpoint wasn't found, which means this breakpoint is the first for this file
	if (!parentBreakpoint) {
		parentBreakpoint = [WCBreakpoint breakpointWithLineNumber:0 inFile:file];
		[parentBreakpoint setBreakpointType:WCBreakpointTypeFile];
		[[[[self project] projectBreakpoint] mutableChildNodes] addObject:parentBreakpoint];
		[[self outlineView] expandItem:[(NSTreeController *)[[[[self project] breakpointsViewController] outlineView] dataSource] treeNodeForRepresentedObject:parentBreakpoint]];
	}
	
	// see if the breakpoint already exists, i.e. its being dragged around in a ruler view
	for (WCBreakpoint *breakpoint in [parentBreakpoint childNodes]) {
		// no need to update
		if (breakpoint == newBreakpoint)
			return;
	}
	
	// add the new breakpoint
	[[parentBreakpoint mutableChildNodes] addObject:newBreakpoint];
}

- (void)_fileDidRemoveBreakpoint:(NSNotification *)note {
	WCFile *file = [note object];
	
	if ([file project] != [self project])
		return;
	
	WCBreakpoint *oldBreakpoint = [[note userInfo] objectForKey:kWCFileBreakpointKey];
	WCBreakpoint *parentBreakpoint = nil;
	
	// find the parent breakpoint that this new breakpoint should be removed from
	for (WCBreakpoint *breakpoint in [[[self project] projectBreakpoint] childNodes]) {
		if ([breakpoint file] == file) {
			parentBreakpoint = breakpoint;
			break;
		}
	}
	
#ifdef DEBUG
    NSAssert(parentBreakpoint != nil, @"could not find parent breakpoint for %@",oldBreakpoint);
#endif
	
	// remove the old breakpoint
	[[parentBreakpoint mutableChildNodes] removeObject:oldBreakpoint];
	
	// remove the parent if that was the last breakpoint
	if ([[parentBreakpoint childNodes] count] == 0)
		[[[[self project] projectBreakpoint] mutableChildNodes] removeObject:parentBreakpoint];
}

- (void)_breakpointIsActiveDidChange:(NSNotification *)note {
	if ([[[note object] file] project] != [self project])
		return;
	
	[[self outlineView] setNeedsDisplay:YES];
}
@end
