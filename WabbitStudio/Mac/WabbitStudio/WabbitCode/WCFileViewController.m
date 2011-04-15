//
//  WCFileViewController.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFileViewController.h"
#import "WCTopBarView.h"
#import "WCFile.h"
//#import "WCRulerView.h"
#import "NoodleLineNumberView.h"
#import "WCTextStorage.h"
#import "WCTextView.h"
#import "WCSymbol.h"
#import "WCSymbolScanner.h"
#import "NSArray+WCExtensions.h"
//#import "WCLineNumberTextView.h"

#import <BWToolkitFramework/BWToolkitFramework.h>

//const CGFloat LargeNumberForText = 1.0e7;

@interface WCFileViewController (Private)
- (void)_setupSymbolsPopUpButton;
@end

@implementation WCFileViewController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_textView = nil;
	_file = nil;
	[_textViewSelectedRangeString release];
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
#ifdef DEBUG
	NSAssert(_scrollView != nil, @"scrollView cannot be nil!");
	NSAssert(_textView != nil, @"textView cannot be nil!");
	NSAssert(_symbolsPopUpButton != nil, @"symbolsPopUpButton cannot be nil!");
	NSAssert(_file != nil, @"file cannot be nil!");
#endif
	
	// for now it has to be done this way, creating everything in code causes the ruler view to flake out
	[[_textView layoutManager] replaceTextStorage:[_file textStorage]];
	
	[_textView setFile:_file];
	[_textView setFileViewController:self];
	
	NoodleLineNumberView *rulerView = [[[NoodleLineNumberView alloc] initWithScrollView:_scrollView] autorelease];
	
	[_scrollView setVerticalRulerView:rulerView];
	[_scrollView setHasVerticalRuler:YES];
	[_scrollView setHasHorizontalRuler:NO];
	[_scrollView setRulersVisible:YES];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_textViewDidChangeSelection:) name:NSTextViewDidChangeSelectionNotification object:_textView];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_symbolScannerDidFinishScanning:) name:kWCSymbolScannerFinishedScanningNotification object:[_file symbolScanner]];
	[_textView setSelectedRange:NSMakeRange(0, 0)];
	[self _setupSymbolsPopUpButton];
}

@synthesize file=_file;
@synthesize textView=_textView;
@synthesize textViewSelectedRangeString=_textViewSelectedRangeString;
@synthesize topBarView=_topBarView;
@synthesize tabViewContext=_tabViewContext;
+ (id)fileViewControllerWithFile:(WCFile *)file; {
	return [self fileViewControllerWithFile:file inTabViewContext:nil];
}

- (id)initWithFile:(WCFile *)file; {
	return [self initWithFile:file tabViewContext:nil];
}

+ (id)fileViewControllerWithFile:(WCFile *)file inTabViewContext:(id <WCTabViewContext>)tabViewContext; {
	return [[[[self class] alloc] initWithFile:file tabViewContext:tabViewContext] autorelease];
}
- (id)initWithFile:(WCFile *)file tabViewContext:(id <WCTabViewContext>)tabViewContext; {
	if (!(self = [super initWithNibName:@"WCFileView" bundle:nil]))
		return nil;
	
	_file = file;
	_tabViewContext = tabViewContext;
	
	return self;
}

- (NSUndoManager *)undoManagerForTextView:(NSTextView *)textView {
	return [[self file] undoManager];
}

- (void)_symbolScannerDidFinishScanning:(NSNotification *)note {
	[self _setupSymbolsPopUpButton];
}

- (void)_setupSymbolsPopUpButton; {
	[_symbolsPopUpButton removeAllItems];
	
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"SymbolsPopUpButtonMenu"] autorelease];
	NSArray *symbols = [[[self file] symbolScanner] symbols];
	
	for (WCSymbol *symbol in symbols) {
		NSMenuItem *item = [menu addItemWithTitle:[symbol name] action:@selector(_jumpToDefinitionFromSymbolsPopUpButton:) keyEquivalent:@""];
		
		[item setTarget:self];
		[item setImage:[symbol icon]];
		[item setRepresentedObject:symbol];
	}
	
	[_symbolsPopUpButton setMenu:menu];
	[_symbolsPopUpButton selectItemAtIndex:[symbols symbolIndexForLocation:[[self textView] selectedRange].location]];
}

- (void)_textViewDidChangeSelection:(NSNotification *)note {
	NSRange range = [[self textView] selectedRange];
	NSUInteger lineNumber = [[[self file] textStorage] lineNumberForCharacterIndex:range.location];
	NSUInteger lineStart = [[[self file] textStorage] lineStartIndexForLineNumber:lineNumber];
	
	[self setTextViewSelectedRangeString:[NSString stringWithFormat:@"%u:%u",++lineNumber,NSMaxRange(range)-lineStart]];
	
	[_symbolsPopUpButton selectItemAtIndex:[[[[self file] symbolScanner] symbols] symbolIndexForLocation:range.location]];
}

- (void)_jumpToDefinitionFromSymbolsPopUpButton:(id)sender {
	[[self textView] jumpToSymbol:[sender representedObject]];
}
@end
