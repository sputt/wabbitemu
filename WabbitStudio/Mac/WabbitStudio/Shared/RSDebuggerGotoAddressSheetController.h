//
//  RSDebuggerMemoryGotoAddressSheetController.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#import "RSGotoAddressControllerProtocol.h"


@class RSDebuggerMemoryViewController;

@interface RSDebuggerGotoAddressSheetController : NSWindowController {
@private
    uint16_t _address;
	__weak id <RSGotoAddressController> _gotoAddressController;
}
@property (assign,nonatomic) uint16_t address;
@property (readonly,nonatomic) id <RSGotoAddressController> gotoAddressController;

+ (void)presentGotoAddressSheetForGotoAddressController:(id <RSGotoAddressController>)gotoAddressController;
@end
