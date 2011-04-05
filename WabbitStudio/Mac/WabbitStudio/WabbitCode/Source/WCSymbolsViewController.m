//
//  WCSymbolsViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSymbolsViewController.h"
#import "WCProject.h"
#import "WCIconTextFieldCell.h"
#import "WCFile.h"
#import "WCSymbol.h"

@implementation WCSymbolsViewController

#pragma mark *** Subclass Overrides ***
- (void)dealloc {
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"WCSymbolsView";
}

- (void)loadView {
	[super loadView];
	
	[[self tableView] setDoubleAction:@selector(_symbolsOutlineViewDoubleAction:)];
	[[self tableView] setTarget:[self project]];
}

- (NSArray *)selectedObjects {
	return [(NSArrayController *)[[self tableView] dataSource] selectedObjects];
}
- (void)setSelectedObjects:(NSArray *)selectedObjects {
	[(NSArrayController *)[[self tableView] dataSource] setSelectedObjects:selectedObjects];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSTableViewDelegate
- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	WCSymbol *symbol = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	[cell setIcon:[symbol icon]];
}

- (NSString *)tableView:(NSTableView *)tableView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row mouseLocation:(NSPoint)mouseLocation {
	WCSymbol *symbol = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	if ([symbol symbolType] == WCSymbolEquateType)
		return [NSString stringWithFormat:NSLocalizedString(@"%@ - %@\nName: %@\nValue: %@", @"equate symbol tooltip"),[[symbol file] name],NSStringFromRange([symbol symbolRange]),[symbol name],[symbol symbolValue]];
	return [NSString stringWithFormat:NSLocalizedString(@"%@ - %@\nName: %@", @"basic symbol tooltip"),[[symbol file] name],NSStringFromRange([symbol symbolRange]),[symbol name]];
}

- (BOOL)tableView:(NSTableView *)tableView shouldEditTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	return NO;
}

- (BOOL)tableView:(NSTableView *)tableView shouldShowCellExpansionForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	return NO;
}
@synthesize tableView=_tableView;

@end
