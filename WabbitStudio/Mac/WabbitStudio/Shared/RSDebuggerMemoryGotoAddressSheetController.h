//
//  RSDebuggerMemoryGotoAddressSheetController.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>


@class RSDebuggerMemoryViewController;

@interface RSDebuggerMemoryGotoAddressSheetController : NSWindowController {
@private
    uint16_t _address;
	__weak RSDebuggerMemoryViewController *_tableViewController;
}
@property (assign,nonatomic) uint16_t address;
@property (readonly,nonatomic) RSDebuggerMemoryViewController *tableViewController;

+ (void)presentGotoAddressSheetForTableViewController:(RSDebuggerMemoryViewController *)tableViewController;
@end
