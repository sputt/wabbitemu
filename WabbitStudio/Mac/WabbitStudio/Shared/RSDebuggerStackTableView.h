//
//  RSDebuggerStackTableView.h
//  WabbitStudio
//
//  Created by William Towe on 5/6/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTableView.h>


extern NSString *const kRSDebuggerStackTableColumnAddressIdentifier;
extern NSString *const kRSDebuggerStackTableColumnStackIdentifier;

@interface RSDebuggerStackTableView : NSTableView {
@private
    
}

- (IBAction)setStackPointerToSelectedAddress:(id)sender;
- (IBAction)gotoAddress:(id)sender;
- (IBAction)gotoStackPointer:(id)sender;

@end
