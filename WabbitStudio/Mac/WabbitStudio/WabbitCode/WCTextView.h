//
//  WCTextView.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTextView.h>


@class WCSyntaxHighlighter,WCFile,WCLineHighlighter,WCSymbol,WCFileViewController,WCFindBarViewController;

@interface WCTextView : NSTextView <NSUserInterfaceValidations> {
@private
	__weak WCFileViewController *_fileViewController; // back pointer to the file view controller, if we have one
	__weak WCFile *_file; // the file we are displaying
    WCSyntaxHighlighter *_syntaxHighlighter; // handles the coloring of keywords and symbols
	WCLineHighlighter *_lineHighlighter; // handles the line highlight
	__weak WCFindBarViewController *_findBarViewController;
	NSTimer *_mouseMovedTimer;
}
@property (assign,nonatomic) WCFile *file;
@property (readonly,nonatomic) NSString *currentSymbolString;
@property (assign,nonatomic) WCFileViewController *fileViewController;
@property (readonly,nonatomic) WCSyntaxHighlighter *syntaxHighlighter;
@property (assign,nonatomic) WCFindBarViewController *findBarViewController;

- (void)jumpToSymbol:(WCSymbol *)symbol;
- (NSString *)symbolStringForRange:(NSRange)range;
- (NSRange)symbolRangeForRange:(NSRange)range;

- (IBAction)jumpToDefinition:(id)sender;
- (IBAction)jumpToNextBuildMessage:(id)sender;
- (IBAction)jumpToPreviousBuildMessage:(id)sender;

- (IBAction)commentOrUncomment:(id)sender;
- (IBAction)blockCommentOrUncomment:(id)sender;
- (IBAction)shiftLeft:(id)sender;
- (IBAction)shiftRight:(id)sender;
- (IBAction)useSelectionForFindInProject:(id)sender;
- (IBAction)toggleWrapLines:(id)sender;
- (IBAction)gotoLine:(id)sender;
- (IBAction)addBreakpointAtCurrentLine:(id)sender;

- (IBAction)openInSeparateEditor:(id)sender;
- (IBAction)revealInProjectView:(id)sender;

@end
