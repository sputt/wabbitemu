//
//  WCFileViewController.h
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#import <AppKit/NSPopUpButton.h>
#import "WCTabViewContextProtocol.h"


@class WCTopBarView,WCFile,WCTextView,BWAnchoredButtonBar,WCLineNumberTextView;

@interface WCFileViewController : NSViewController <NSTextViewDelegate,NSMenuDelegate> {
@private
	IBOutlet NSScrollView *_scrollView;
	IBOutlet WCTextView *_textView;
	IBOutlet NSPopUpButton *_symbolsPopUpButton;
	IBOutlet NSView *_topBarView;
	
	__weak WCFile *_file;
	__weak id <WCTabViewContext> _tabViewContext;
	NSString *_textViewSelectedRangeString;
	NSInteger _currentSymbolIndex;
}

@property (assign,nonatomic) WCFile *file;
@property (assign,nonatomic) WCTextView *textView;
@property (copy,nonatomic) NSString *textViewSelectedRangeString;
@property (readonly,nonatomic) NSView *topBarView;
@property (readonly,nonatomic) id <WCTabViewContext> tabViewContext;

+ (id)fileViewControllerWithFile:(WCFile *)file;
- (id)initWithFile:(WCFile *)file;

+ (id)fileViewControllerWithFile:(WCFile *)file inTabViewContext:(id <WCTabViewContext>)tabViewContext;
- (id)initWithFile:(WCFile *)file tabViewContext:(id <WCTabViewContext>)tabViewContext;
@end
