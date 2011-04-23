//
//  WCFileViewController.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFileViewController.h"
#import "WCFile.h"
#import "NoodleLineNumberView.h"
#import "WCTextStorage.h"
#import "WCTextView.h"
#import "WCSymbol.h"
#import "WCSymbolScanner.h"
#import "NSArray+WCExtensions.h"


@interface WCFileViewController ()
- (void)_setupSymbolsMenuAndAdjustNumberOfItems:(BOOL)adjustNumberOfItems;
@end

@implementation WCFileViewController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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
	[self _setupSymbolsMenuAndAdjustNumberOfItems:YES];
	[_textView setSelectedRange:NSMakeRange(0, 0)];
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
	[self _setupSymbolsMenuAndAdjustNumberOfItems:YES];
}

- (void)_textViewDidChangeSelection:(NSNotification *)note {
	NSRange range = [[self textView] selectedRange];
	NSUInteger lineNumber = [[[self file] textStorage] lineNumberForCharacterIndex:range.location];
	NSUInteger lineStart = [[[self file] textStorage] lineStartIndexForLineNumber:lineNumber];
	
	[self setTextViewSelectedRangeString:[NSString stringWithFormat:@"%u:%u",++lineNumber,NSMaxRange(range)-lineStart]];
	
	[self _setupSymbolsMenuAndAdjustNumberOfItems:NO];
}

- (void)_setupSymbolsMenuAndAdjustNumberOfItems:(BOOL)adjustNumberOfItems; {
	NSArray *symbols = [[[self file] symbolScanner] symbols];
	NSMenu *menu = [_symbolsPopUpButton menu];
	
	if (adjustNumberOfItems) {
		NSUInteger totalItems = ([symbols count] == 0)?1:[symbols count];
		
		// add items until we are even
		if ([menu numberOfItems] < totalItems) {
			while ([menu numberOfItems] < totalItems)
				[menu addItem:[[[NSMenuItem alloc] init] autorelease]];
		}
		// remove items until we are even
		else if ([menu numberOfItems] > totalItems) {
			while ([menu numberOfItems] > totalItems)
				[menu removeItemAtIndex:0];
		}
		
		if ([symbols count] == 0) {
			[[menu itemAtIndex:0] setTitle:NSLocalizedString(@"No Symbols", @"title for no symbols menu item")];
			[[menu itemAtIndex:0] setImage:nil];
			[[menu itemAtIndex:0] setRepresentedObject:nil];
		}
		else {
			NSUInteger count = 0;
			for (NSMenuItem *item in [menu itemArray]) {
				WCSymbol *symbol = [symbols objectAtIndex:count++];
				
				[item setTitle:[symbol name]];
				[item setImage:[symbol icon]];
				[item setIndentationLevel:([symbol symbolType] == WCSymbolFunctionType)?0:1];
				[item setRepresentedObject:symbol];
				[item setAction:@selector(_jumpToDefinitionFromSymbolsPopUpButton:)];
				[item setTarget:self];
			}
		}
	}

	[_symbolsPopUpButton selectItemAtIndex:[symbols symbolIndexForLocation:[_textView selectedRange].location]];
}

- (void)_jumpToDefinitionFromSymbolsPopUpButton:(id)sender {
	[[self textView] jumpToSymbol:[sender representedObject]];
}
@end
