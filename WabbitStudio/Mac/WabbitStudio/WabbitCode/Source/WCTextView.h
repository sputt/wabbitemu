//
//  WCTextView.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTextView.h>


@class WCSyntaxHighlighter,WCFile,WCLineHighlighter,WCSymbol,WCFileViewController;

@interface WCTextView : NSTextView {
@private
	WCFileViewController *_fileViewController; // back pointer to the file view controller, if we have one
	WCFile *_file; // the file we are displaying
    WCSyntaxHighlighter *_syntaxHighlighter; // handles the coloring of keywords and symbols
	WCLineHighlighter *_lineHighlighter; // handles the line highlight
}
@property (assign,nonatomic) WCFile *file;
@property (readonly,nonatomic) NSString *currentSymbolString;
@property (assign,nonatomic) WCFileViewController *fileViewController;

- (void)jumpToSymbol:(WCSymbol *)symbol;

- (IBAction)jumpToDefinition:(id)sender;
- (IBAction)commentOrUncomment:(id)sender;
- (IBAction)blockCommentOrUncomment:(id)sender;
- (IBAction)shiftLeft:(id)sender;
- (IBAction)shiftRight:(id)sender;
@end
