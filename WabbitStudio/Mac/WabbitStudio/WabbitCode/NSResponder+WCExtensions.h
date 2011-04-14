//
//  NSResponder+WCExtensions.h
//  WabbitCode
//
//  Created by William Towe on 1/7/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import <AppKit/NSResponder.h>


@interface NSResponder (WCExtensions)
- (IBAction)approveModalWindowAction:(id)sender;
- (IBAction)dismissModalWindowAction:(id)sender;

- (IBAction)approveModalSheetAction:(id)sender;
- (IBAction)dismissModalSheetAction:(id)sender;
@end
