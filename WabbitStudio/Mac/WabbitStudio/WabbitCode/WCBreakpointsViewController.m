//
//  WCBreakpointsViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/19/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBreakpointsViewController.h"
#import "WCDoEverythingTextFieldCell.h"
#import "WCBreakpoint.h"
#import "WCProject.h"
#import "WCProjectFile.h"


@implementation WCBreakpointsViewController

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_outlineView setDelegate:nil];
	[_outlineView setDataSource:nil];
	[_breakpoint release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"WCBreakpointsView";
}

- (void)loadView {
	_breakpoint = [[WCBreakpoint alloc] initWithLineNumber:0 file:[[self project] projectFile]];
	[_breakpoint setBreakpointType:WCBreakpointTypeProject];
	
	for (WCFile *file in [[self project] textFiles]) {
		if ([[file allBreakpoints] count] == 0)
			continue;
		
		WCBreakpoint *parent = [WCBreakpoint breakpointWithLineNumber:0 inFile:file];
		[parent setBreakpointType:WCBreakpointTypeFile];
		[[_breakpoint mutableChildNodes] addObject:parent];
		
		for (WCBreakpoint *breakpoint in [file allBreakpointsSortedByLineNumber])
			[[parent mutableChildNodes] addObject:breakpoint];
	}
	
	[super loadView];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileNumberOfBreakpointsDidChange:) name:kWCFileNumberOfBreakpointsDidChangeNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_breakpointIsActiveDidChange:) name:kWCBreakpointIsActiveDidChangeNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_numberOfFilesDidChange:) name:kWCProjectNumberOfFilesDidChangeNotification object:[self project]];
	
	[[self outlineView] setDoubleAction:@selector(_breakpointsOutlineViewDoubleAction:)];
	[[self outlineView] setTarget:[self project]];
	
	[[self outlineView] expandItem:_breakpoint expandChildren:YES];
}

- (NSArray *)selectedObjects {
	if ([[self outlineView] selectedRow] == -1)
		return nil;
	return [NSArray arrayWithObject:[[self outlineView] itemAtRow:[[self outlineView] selectedRow]]];
}
- (void)setSelectedObjects:(NSArray *)selectedObjects {
	NSMutableIndexSet *rowIndexes = [NSMutableIndexSet indexSet];
	
	for (id object in selectedObjects) {
		[rowIndexes addIndex:[[self outlineView] rowForItem:object]];
	}
	
	[[self outlineView] selectRowIndexes:rowIndexes byExtendingSelection:NO];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSOutlineViewDelegate
- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(WCDoEverythingTextFieldCell *)cell forTableColumn:(NSTableColumn *)tableColumn item:(WCBreakpoint *)item {
	
	[cell setIcon:[item icon]];
	if ([item breakpointType] == WCBreakpointTypeLine)
		[cell setIconSize:[[item icon] size]];
	else
		[cell setIconSize:NSMakeSize(16.0, 16.0)];
	
	if ([item breakpointType] == WCBreakpointTypeProject ||
		[item breakpointType] == WCBreakpointTypeLine)
		[cell setBadgeCount:0];
	else
		[cell setBadgeCount:[[item childNodes] count]];
	
	if ([item breakpointType] == WCBreakpointTypeProject) {
		NSMutableAttributedString *attributedString = [[[cell attributedStringValue] mutableCopy] autorelease];
		[attributedString applyFontTraits:NSBoldFontMask range:NSMakeRange(0, [[attributedString string] length])];
		[cell setAttributedStringValue:attributedString];
	}
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}

- (NSString *)outlineView:(NSOutlineView *)outlineView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn item:(id)item mouseLocation:(NSPoint)mouseLocation {
	return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldShowCellExpansionForTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}

#pragma mark NSOutlineViewDataSource
- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(WCBreakpoint *)item {
	if (item == nil)
		return _breakpoint;
	return [[item childNodes] objectAtIndex:index];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(WCBreakpoint *)item {
	if ([item breakpointType] == WCBreakpointTypeLine)
		return NO;
	return YES;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(WCBreakpoint *)item {
	if (item == nil)
		return 1;
	return [[item childNodes] count];
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(WCBreakpoint *)item {
	return [item name];
}

@synthesize outlineView=_outlineView;

- (void)_fileNumberOfBreakpointsDidChange:(NSNotification *)note {
	if ([[note object] project] != [self project])
		return;
	
	WCFile *file = [note object];
	
	for (WCBreakpoint *bp in [_breakpoint childNodes]) {
		if ([bp file] == file) {
			if ([[file allBreakpoints] count] == 0) {
				[[_breakpoint mutableChildNodes] removeObject:bp];
				[[self outlineView] reloadData];
				return;
			}
			else {
				[[bp mutableChildNodes] removeAllObjects];
				
				for (WCBreakpoint *cbp in [file allBreakpointsSortedByLineNumber])
					[[bp mutableChildNodes] addObject:cbp];
				
				[[self outlineView] reloadItem:bp reloadChildren:YES];
				return;
			}
		}
	}
	
	WCBreakpoint *parent = [WCBreakpoint breakpointWithLineNumber:0 inFile:file];
	[parent setBreakpointType:WCBreakpointTypeFile];
	[[_breakpoint mutableChildNodes] addObject:parent];
	
	for (WCBreakpoint *cbp in [file allBreakpointsSortedByLineNumber])
		[[parent mutableChildNodes] addObject:cbp];
	
	[[self outlineView] reloadData];
	[[self outlineView] expandItem:parent];
}

- (void)_breakpointIsActiveDidChange:(NSNotification *)note {
	if ([[[note object] file] project] != [self project])
		return;
	
	[[self outlineView] reloadItem:[note object]];
}

- (void)_numberOfFilesDidChange:(NSNotification *)note {
	[[_breakpoint mutableChildNodes] removeAllObjects];
	
	for (WCFile *file in [[self project] textFiles]) {
		if ([[file allBreakpoints] count] == 0)
			continue;
		
		WCBreakpoint *parent = [WCBreakpoint breakpointWithLineNumber:0 inFile:file];
		[parent setBreakpointType:WCBreakpointTypeFile];
		[[_breakpoint mutableChildNodes] addObject:parent];
		
		for (WCBreakpoint *breakpoint in [file allBreakpointsSortedByLineNumber])
			[[parent mutableChildNodes] addObject:breakpoint];
	}
	
	[[self outlineView] reloadData];
}
@end
