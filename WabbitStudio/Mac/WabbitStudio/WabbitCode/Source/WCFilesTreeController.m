//
//  WCFilesTreeController.m
//  WabbitStudio
//
//  Created by William Towe on 3/21/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFilesTreeController.h"
#import "WCFilesOutlineView.h"
#import "WCProject.h"
#import "WCProjectFile.h"
#import "NSTreeController+WCExtensions.h"
#import "NSTreeNode+WCExtensions.h"
#import "WCAddFilesToProjectViewController.h"
#import "WCGeneralPerformer.h"


@implementation WCFilesTreeController
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
    [super dealloc];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSOutlineViewDataSource
/*
- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item {
	return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item {
	return NO;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item {
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item {
	return nil;
}
 */
- (NSDragOperation)outlineView:(NSOutlineView *)outlineView validateDrop:(id <NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index {
	
	if (index == NSOutlineViewDropOnItemIndex || !item)
		return NSDragOperationNone;
	
	NSArray *types = [[info draggingPasteboard] types];
	
	if ([types containsObject:WCFilesOutlineViewPboardType]) {
		NSArray *indexPaths = [NSKeyedUnarchiver unarchiveObjectWithData:[[info draggingPasteboard] dataForType:WCFilesOutlineViewPboardType]];
		NSMutableArray *realItems = [[self treeNodesAtIndexPaths:indexPaths] valueForKeyPath:@"representedObject"];
		
		for (WCTreeNode *node in realItems) {
			if ([node isEqualTo:[item representedObject]] ||
				[[item representedObject] isDescendantOfNode:node])
				return NSDragOperationNone;
			else if ([realItems count] == 1 &&
					 [[item representedObject] isEqualTo:[node parentNode]] &&
					 ([[[node parentNode] childNodes] indexOfObject:node] == index ||
					  [[[node parentNode] childNodes] indexOfObject:node] == --index))
				return NSDragOperationNone;
		}
		
		return NSDragOperationMove;
	}
	else if ([types containsObject:NSFilenamesPboardType] && [info draggingSource] != outlineView) {
		
		return NSDragOperationCopy;
	}
	return NSDragOperationNone;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView acceptDrop:(id <NSDraggingInfo>)info item:(id)item childIndex:(NSInteger)index {
	
	NSArray *types = [[info draggingPasteboard] types];
	
	if ([types containsObject:WCFilesOutlineViewPboardType]) {
		NSArray *indexPaths = [NSKeyedUnarchiver unarchiveObjectWithData:[[info draggingPasteboard] dataForType:WCFilesOutlineViewPboardType]];
		NSArray *treeNodes = [self treeNodesAtIndexPaths:indexPaths];
		
		[self moveNodes:treeNodes toIndexPath:[[item indexPath] indexPathByAddingIndex:index]];
	}
	else if ([types containsObject:NSFilenamesPboardType]) {
		NSArray *filePaths = [[info draggingPasteboard] propertyListForType:NSFilenamesPboardType];
		NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"Add Files to Project", @"add files to project alert title") defaultButton:NSLocalizedString(@"Add", @"add files to project alert default button title") alternateButton:NSLocalizedString(@"Cancel", @"add files to project alert alternate button title") otherButton:nil informativeTextWithFormat:NSLocalizedString(@"Choose options for adding these files to the project", @"add files to project alert message text")];
		[alert setAccessoryView:[[[[WCAddFilesToProjectViewController alloc] init] autorelease] view]];
		[alert beginSheetModalForWindow:[outlineView window] modalDelegate:self didEndSelector:@selector(alertDidEnd:code:info:) contextInfo:(void *)[[NSDictionary dictionaryWithObjectsAndKeys:filePaths,@"filePaths",[item indexPath],@"indexPath",[NSNumber numberWithInteger:index],@"index",nil] retain]];
		
	}
	return YES;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView writeItems:(NSArray *)items toPasteboard:(NSPasteboard *)pboard {
	NSArray *realItems = [items valueForKeyPath:@"representedObject"];
	NSMutableArray *types = [[[pboard types] mutableCopy] autorelease];
	
	[types addObject:WCFilesOutlineViewPboardType];
	[types addObject:NSFilenamesPboardType];
	
	[pboard declareTypes:types owner:nil];
	
	[pboard setData:[NSKeyedArchiver archivedDataWithRootObject:[items valueForKeyPath:@"indexPath"]] forType:WCFilesOutlineViewPboardType];
	[pboard setPropertyList:[realItems valueForKeyPath:@"filePath"] forType:NSFilenamesPboardType];
	
	return YES;
}
#pragma mark *** Private Methods ***
#pragma mark Callbacks
- (void)alertDidEnd:(NSAlert *)alert code:(NSInteger)code info:(void *)info {
	// we had to retain the dictionary passed in, autorelease it or we will leak
	NSDictionary *dict = (NSDictionary *)[(id)info autorelease];
	
	if (code == NSAlertAlternateReturn)
		return;
	
	[[WCGeneralPerformer sharedPerformer] addFilePaths:[dict objectForKey:@"filePaths"] toFile:[[self treeNodeAtIndexPath:[dict objectForKey:@"indexPath"]] representedObject] atIndex:[[dict objectForKey:@"index"] unsignedIntegerValue]];
}
@end
