//
//  WCBuildDefinesArrayController.m
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildDefinesArrayController.h"
#import "WCBuildDefine.h"


@implementation WCBuildDefinesArrayController

- (void)add:(id)sender {
	WCBuildDefine *bd = [WCBuildDefine defineWithName:NSLocalizedString(@"NEWDEFINE", @"new define default name")];
	
	[self insertObject:bd atArrangedObjectIndex:0];
	
	[oTableView editColumn:0 row:0 withEvent:nil select:YES];
}

@end
