// RSRTVArrayController.h
//
// RSRTV stands for Red Sweater Reordering Table View Controller.
//
// A simple array controller subclass designed to manage row reordering in a table 
// view. 
//
// Based on code from Apple's DragNDropOutlineView example, which granted 
// unlimited modification and redistribution rights, provided Apple not be held legally liable.
//
// Differences between this file and the original are © 2006 Red Sweater Software.
//
// You are granted a non-exclusive, unlimited license to use, reproduce, modify and 
// redistribute this source code in any form provided that you agree to NOT hold liable 
// Red Sweater Software or Daniel Jalkut for any damages caused by such use.
//

#import <Cocoa/Cocoa.h>

@interface RSRTVArrayController : NSArrayController <NSTableViewDataSource>
{
    IBOutlet NSTableView* oTableView;
	
	BOOL mDraggingEnabled;
}

// Allow dragging (reordering) to be temporarily disabled
- (BOOL) draggingEnabled;
- (void) setDraggingEnabled: (BOOL) flag;

// Utility methods
-(void)moveObjectsInArrangedObjectsFromIndexes:(NSIndexSet *)indexSet toIndex:(NSUInteger)index;
-(void)copyObjectsInArrangedObjectsFromIndexes:(NSIndexSet*)indexSet toIndex:(NSUInteger)insertIndex;
- (NSIndexSet *)indexSetFromRows:(NSArray *)rows;
- (NSInteger)rowsAboveRow:(NSInteger)row inIndexSet:(NSIndexSet *)indexSet;

@end
