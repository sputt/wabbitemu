//
//  RSMemoryTableView.h
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTableView.h>


@interface RSDebuggerMemoryTableView : NSTableView {
@private
    
}

- (IBAction)gotoAddress:(id)sender;
- (IBAction)gotoProgramCounter:(id)sender;

@end
