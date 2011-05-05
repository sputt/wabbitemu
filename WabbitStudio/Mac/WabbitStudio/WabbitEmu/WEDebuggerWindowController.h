//
//  WEDebuggerWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>


@class RSDebuggerDetailsViewController,WECalculatorDocument,RSDebuggerMemoryViewController;

@interface WEDebuggerWindowController : NSWindowController <NSWindowDelegate,NSSplitViewDelegate> {
@private
	IBOutlet NSSplitView *_verticalSplitView;
	IBOutlet NSSplitView *_horizontalSplitView;
	
    RSDebuggerDetailsViewController *_detailsViewController;
	RSDebuggerMemoryViewController *_memoryViewController;
}
@property (readonly,nonatomic) WECalculatorDocument *calculatorDocument;
@end
