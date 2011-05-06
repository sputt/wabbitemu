//
//  WEDebuggerWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>


@class RSDebuggerDetailsViewController,WECalculatorDocument,RSDebuggerMemoryViewController,RSDebuggerDisassemblyViewController,RSDebuggerStackViewController;

@interface WEDebuggerWindowController : NSWindowController <NSWindowDelegate,NSSplitViewDelegate,NSToolbarDelegate> {
@private
	IBOutlet NSSplitView *_verticalSplitView;
	IBOutlet NSSplitView *_horizontalSplitView;
	IBOutlet NSView *_leftBottomDummyView;
	IBOutlet NSView *_rightBottomDummyView;
	
    RSDebuggerDetailsViewController *_detailsViewController;
	RSDebuggerMemoryViewController *_memoryViewController;
	RSDebuggerDisassemblyViewController *_disassemblyViewController;
	RSDebuggerStackViewController *_stackViewController;
}
@property (readonly,nonatomic) WECalculatorDocument *calculatorDocument;

- (IBAction)step:(id)sender;
- (IBAction)stepOver:(id)sender;

@end
