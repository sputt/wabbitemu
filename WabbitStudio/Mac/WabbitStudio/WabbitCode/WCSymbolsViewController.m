//
//  WCSymbolsViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSymbolsViewController.h"
#import "WCProject.h"
#import "WCBadgedTextFieldCell.h"
#import "WCFile.h"
#import "WCSymbol.h"
#import "WCTextStorage.h"
#import "WCSymbolScanner.h"
#import "NSObject+WCExtensions.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"


@interface WCSymbolsViewController ()
@property (readonly,nonatomic) NSMutableArray *symbols;
@property (readonly,nonatomic) NSMutableArray *filteredSymbols;

- (void)_resetFilterString;
@end

@implementation WCSymbolsViewController

#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_outlineView setDelegate:nil];
	[_outlineView setDataSource:nil];
	[_filterString release];
	[_filteredSymbols release];
	[_symbols release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"WCSymbolsView";
}

- (void)loadView {	
	for (WCFile *file in [[self project] textFiles]) {
		WCSymbol *symbol = [WCSymbol symbolWithName:nil ofType:WCSymbolFileType inFile:file withRange:NSMakeRange(0, 0)];
		
		[[self symbols] addObject:symbol];
		[[symbol mutableChildNodes] addObjectsFromArray:[[[symbol file] symbolScanner] symbols]];
		[[self filteredSymbols] addObject:symbol];
	}
	
	[super loadView];
	
	[self setupNotificationObserving];
	
	[[self outlineView] setDoubleAction:@selector(symbolsOutlineViewDoubleClick:)];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_symbolScannerDidFinishScanning:)),kNSObjectSelectorKey,kWCSymbolScannerFinishedScanningNotification,kNSObjectNotificationNameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_projectNumberOfFilesDidChange:)),kNSObjectSelectorKey,kWCProjectNumberOfFilesDidChangeNotification,kNSObjectNotificationNameKey,[self project],kNSObjectNotificationObjectKey, nil], nil];
}

- (NSArray *)selectedObjects {
	if ([[self outlineView] selectedRow] == -1)
		return nil;
	return [NSArray arrayWithObject:[[self outlineView] itemAtRow:[[self outlineView] selectedRow]]];
}
- (void)setSelectedObjects:(NSArray *)selectedObjects {
	NSMutableIndexSet *rowIndexes = [NSMutableIndexSet indexSet];
	
	for (id object in selectedObjects) {
		[rowIndexes addIndex:[[self outlineView] rowForItem:object]];
	}
	
	[[self outlineView] selectRowIndexes:rowIndexes byExtendingSelection:NO];
}

#pragma mark *** Protocol Overrides ***
#pragma mark NSOutlineViewDelegate
- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	
	[cell setIcon:[item icon]];
	[cell setIconSize:NSMakeSize(16.0, 16.0)];
	[cell setBadgeCount:[[item childNodes] count]];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return NO;
}

- (NSString *)outlineView:(NSOutlineView *)outlineView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn item:(id)item mouseLocation:(NSPoint)mouseLocation {
	
	if ([item symbolType] == WCSymbolFileType) {
		NSArray *symbols = [[[item file] symbolScanner] symbols];
		NSUInteger defines = 0, macros = 0, equates = 0, labels = 0;
		for (WCSymbol *symbol in symbols) {
			switch ([symbol symbolType]) {
				case WCSymbolDefineType:
					defines++;
					break;
				case WCSymbolMacroType:
					macros++;
					break;
				case WCSymbolEquateType:
					equates++;
					break;
				case WCSymbolLabelType:
					labels++;
					break;
				case WCSymbolFileType:
				default:
					break;
			}
		}
		return [NSString stringWithFormat:NSLocalizedString(@"File: %@ - %lu symbol(s)\nDefines: %lu\nMacros: %lu\nEquates: %lu\nLabels: %lu", @"file symbol tooltip"),[item name],[symbols count],defines,macros,equates,labels];
	}
	else if ([item symbolType] == WCSymbolEquateType)
		return [NSString stringWithFormat:NSLocalizedString(@"File: %@ - line %lu\nName: %@\nValue: %@", @"equate and macro symbol tooltip"),[[item file] name],[[[item file] textStorage] lineStartIndexForCharacterIndex:[item symbolRange].location],[item name],[item symbolValue]];
	return [NSString stringWithFormat:NSLocalizedString(@"File: %@ - line %lu\nName: %@", @"other symbol tooltip"),[[item file] name],[[[item file] textStorage] lineStartIndexForCharacterIndex:[item symbolRange].location],[item name]];
}
#pragma mark NSOutlineViewDataSource
- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item {
	if (!item)
		return [[self filteredSymbols] objectAtIndex:index];
	return [[item childNodes] objectAtIndex:index];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item {
	return (![item isLeaf]);
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item {
	if (!item)
		return [[self filteredSymbols] count];
	return [[item childNodes] count];
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item {
	return [item name];
}
#pragma mark *** Public Methods ***
#pragma mark Accessors
@synthesize outlineView=_outlineView;
@synthesize filterString=_filterString;
#pragma mark IBActions
- (IBAction)filterSymbols:(id)sender; {
	if (![self filterString] || ![[self filterString] length]) {
		[self _resetFilterString];
		return;
	}
	
	[[self filteredSymbols] removeAllObjects];
	
	NSString *filterString = [self filterString];
	
	for (WCSymbol *symbol in [self symbols]) {
		NSMutableArray *mSymbols = [NSMutableArray array];
		for (WCSymbol *cSymbol in [[[symbol file] symbolScanner] symbols]) {
			if ([[cSymbol name] rangeOfString:filterString options:NSCaseInsensitiveSearch].location != NSNotFound)
				[mSymbols addObject:cSymbol];
		}
		
		if ([mSymbols count]) {
			[[symbol mutableChildNodes] removeAllObjects];
			[[symbol mutableChildNodes] addObjectsFromArray:mSymbols];
			[[self filteredSymbols] addObject:symbol];
		}
	}
	
	[[self outlineView] reloadData];
	
	for (WCSymbol *symbol in [self filteredSymbols])
		[[self outlineView] expandItem:symbol];
}

- (IBAction)symbolsOutlineViewSingleClick:(id)sender; {
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithSingleClick)
		return;
	
	[[self project] jumpToObjects:[self selectedObjects]];
}
- (IBAction)symbolsOutlineViewDoubleClick:(id)sender; {
	NSArray *selectedObjects = [self selectedObjects];
	
	if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] != WCPreferencesFilesOpenWithDoubleClick)
		return;
	
	[[self project] jumpToObjects:selectedObjects];
}
#pragma mark *** Private Methods ***
- (void)_resetFilterString {
	[self setFilterString:@""];
	
	[[self filteredSymbols] removeAllObjects];
	for (WCSymbol *symbol in _symbols) {
		[[symbol mutableChildNodes] removeAllObjects];
		[[symbol mutableChildNodes] addObjectsFromArray:[[[symbol file] symbolScanner] symbols]];
		[[self filteredSymbols] addObject:symbol];
	}
	[[self outlineView] reloadData];
}
#pragma mark Accessors
@dynamic symbols;
- (NSMutableArray *)symbols {
	if (!_symbols)
		_symbols = [[NSMutableArray alloc] initWithCapacity:[[[self project] textFiles] count]];
	return _symbols;
}
@dynamic filteredSymbols;
- (NSMutableArray *)filteredSymbols {
	if (!_filteredSymbols)
		_filteredSymbols = [[NSMutableArray alloc] initWithCapacity:[[self symbols] count]];
	return _filteredSymbols;
}
#pragma mark Notifications
- (void)_symbolScannerDidFinishScanning:(NSNotification *)note {
	if ([[[note object] file] project] != [self project])
		return;
	
	WCFile *file = [[note object] file];
	
	for (WCSymbol *symbol in [self symbols]) {
		if ([symbol file] == file) {
			[[symbol mutableChildNodes] removeAllObjects];
			[[symbol mutableChildNodes] addObjectsFromArray:[[file symbolScanner] symbols]];
			[[self outlineView] reloadItem:symbol reloadChildren:YES];
			break;
		}
	}
}

- (void)_projectNumberOfFilesDidChange:(NSNotification *)note {
	[[self symbols] removeAllObjects];
	
	for (WCFile *file in [[self project] textFiles]) {
		[[self symbols] addObject:[WCSymbol symbolWithName:nil ofType:WCSymbolFileType inFile:file withRange:NSMakeRange(0, 0)]];
	}
	
	[_filteredSymbols removeAllObjects];
	
	for (WCSymbol *symbol in _symbols) {
		[[symbol mutableChildNodes] addObjectsFromArray:[[[symbol file] symbolScanner] symbols]];
		[[self filteredSymbols] addObject:symbol];
	}
	
	[[self outlineView] reloadData];
}
@end
