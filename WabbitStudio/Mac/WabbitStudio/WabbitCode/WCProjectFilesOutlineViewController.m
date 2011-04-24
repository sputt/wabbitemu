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

@implementation WCProjectFilesOutlineViewController
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	//[[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	//[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_updateProjectFileRow:) name:kWCProjectNumberOfBuildTargetsDidChangeNotification object:[self project]];
	
	[_outlineView setDoubleAction:@selector(_outlineViewDoubleClick:)];
	[_outlineView setTarget:[self project]];
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
	[cell setIconSize:NSMakeSize(16.0, 16.0)];
	
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
#pragma mark *** Private Methods ***
/*
- (void)_updateProjectFileRow:(NSNotification *)note {
	[[self outlineView] setNeedsDisplayInRect:[[self outlineView] rectOfRow:0]];
}
 */
@end
