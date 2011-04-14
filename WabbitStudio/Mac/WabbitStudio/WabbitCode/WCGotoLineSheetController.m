//
//  WCGotoLineSheetController.m
//  WabbitStudio
//
//  Created by William Towe on 4/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGotoLineSheetController.h"
#import "WCTextView.h"
#import "WCTextStorage.h"
#import "WCFile.h"

@interface WCGotoLineSheetController ()
- (id)_initWithTextView:(WCTextView *)textView;
@end

@implementation WCGotoLineSheetController

- (NSString *)windowNibName {
	return @"WCGotoLineSheet";
}

- (void)dealloc {
	[_lineNumber release];
    [super dealloc];
}

+ (void)presentGotoLineSheetForTextView:(WCTextView *)textView; {
	WCGotoLineSheetController *controller = [[[self class] alloc] _initWithTextView:textView];
	
	[[NSApplication sharedApplication] beginSheet:[controller window] modalForWindow:[textView window] modalDelegate:controller didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

@synthesize lineNumber=_lineNumber;
@synthesize lineOrCharacter=_lineOrCharacter;

- (id)_initWithTextView:(WCTextView *)textView; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_textView = textView;
	_lineOrCharacter = WCGotoLineOrCharacterLine;
	
	NSRange range = [textView selectedRange];
	NSUInteger location = range.location;
	
	if (_lineOrCharacter == WCGotoLineOrCharacterCharacter) {
		_lineNumber = [[NSNumber numberWithUnsignedInteger:location] copy];
	}
	else {
		_lineNumber = [[NSNumber numberWithUnsignedInteger:[[[textView file] textStorage] lineNumberForCharacterIndex:location]+1] copy];
	}
	
	return self;
}

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	if (code != NSOKButton)
		return;
	
	NSRange range = NSMakeRange([_textView selectedRange].location, [_textView selectedRange].length);
	if (_lineOrCharacter == WCGotoLineOrCharacterCharacter) {
		NSUInteger location = [[self lineNumber] unsignedIntegerValue];
		
		if (location >= [[_textView string] length])
			location = [[_textView string] length] - 1;
		
		range.location = location;
		range.length = 0;
	}
	else {
		NSUInteger line = [[self lineNumber] unsignedIntegerValue];
		
		if (line >= [[[_textView file] textStorage] numberOfLines])
			line = [[[_textView file] textStorage] numberOfLines];
		
		range.location = [[[_textView file] textStorage] lineStartIndexForLineNumber:--line];
		range.length = 0;
	}
	[_textView setSelectedRange:range];
	[_textView scrollRangeToVisible:range];
}
@end
