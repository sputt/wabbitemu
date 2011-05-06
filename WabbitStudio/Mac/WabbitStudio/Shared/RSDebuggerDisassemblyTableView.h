//
//  RSDebuggerDisassemblyTableView.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTableView.h>
#import "RSGotoAddressControllerProtocol.h"


extern NSString *const kDisassemblyTableColumnAddressIdentifier;
extern NSString *const kDisassemblyTableColumnDataIdentifier;
extern NSString *const kDisassemblyTableColumnDisassemblyIdentifier;
extern NSString *const kDisassemblyTableColumnSizeIdentifier;


@interface RSDebuggerDisassemblyTableView : NSTableView {
@private

}

- (IBAction)gotoAddress:(id)sender;
- (IBAction)toggleBreakpoint:(id)sender;

@end
