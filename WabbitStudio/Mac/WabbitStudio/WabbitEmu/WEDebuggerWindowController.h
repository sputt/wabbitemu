//
//  WEDebuggerWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>


@class RSDebuggerDetailsViewController,WECalculatorDocument;

@interface WEDebuggerWindowController : NSWindowController <NSWindowDelegate> {
@private
	IBOutlet NSSplitView *_verticalSplitView;
	
    RSDebuggerDetailsViewController *_detailsViewController;
}
@property (readonly,nonatomic) WECalculatorDocument *calculatorDocument;
@end
