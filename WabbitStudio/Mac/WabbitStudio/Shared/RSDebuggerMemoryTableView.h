//
//  RSMemoryTableView.h
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTableView.h>


@class RSDebuggerMemoryViewController;

@interface RSDebuggerMemoryTableView : NSTableView {
@private
    IBOutlet RSDebuggerMemoryViewController *_tableViewController;
}

- (IBAction)gotoAddress:(id)sender;

@end
