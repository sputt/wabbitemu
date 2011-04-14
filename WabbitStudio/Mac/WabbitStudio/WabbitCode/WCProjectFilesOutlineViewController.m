//
//  WCProjectFilesOutlineViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectFilesOutlineViewController.h"
#import "WCProject.h"
#import "WCIconTextFieldCell.h"
#import "WCFile.h"
#import "WCAlias.h"
#import "NSTreeController+WCExtensions.h"

@implementation WCProjectFilesOutlineViewController
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	[_outlineView setDoubleAction:@selector(_outlineViewDoubleClick:)];
	[_outlineView setTarget:[self project]];
}

- (NSString *)viewNibName {
	return @"WCProjectFilesOutlineView";
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
}

- (NSString *)outlineView:(NSOutlineView *)outlineView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tc item:(id)item mouseLocation:(NSPoint)mouseLocation {
	return [[[item representedObject] alias] absolutePathForDisplay];
	
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return [[item representedObject] canEditName];
}
#pragma mark *** Public Methods ***

#pragma mark Accessors
@synthesize outlineView=_outlineView;
@end
