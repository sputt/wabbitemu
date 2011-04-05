//
//  WCFileViewController.h
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>


@class WCTopBarView,WCFile,WCTextView,BWAnchoredButtonBar,WCLineNumberTextView;

@interface WCFileViewController : NSViewController <NSTextViewDelegate> {
@private
	IBOutlet NSScrollView *_scrollView;
	IBOutlet WCTextView *_textView;
	IBOutlet NSPopUpButton *_symbolsPopUpButton;
	IBOutlet WCLineNumberTextView *_lineNumberTextView;
	IBOutlet WCTopBarView *_topBarView;
	
	WCFile *_file;
	NSString *_textViewSelectedRangeString;
}

@property (assign,nonatomic) WCFile *file;
@property (assign,nonatomic) WCTextView *textView;
@property (copy,nonatomic) NSString *textViewSelectedRangeString;
@property (readonly,nonatomic) WCTopBarView *topBarView;

+ (id)fileViewControllerWithFile:(WCFile *)file;
- (id)initWithFile:(WCFile *)file;
@end
