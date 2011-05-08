//
//  WCSymbol.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSymbol.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "NSColor-NTExtensions.h"
#import "WCFile.h"
#import "WCAlias.h"
#import "WCTextStorage.h"


@implementation WCSymbol

- (void)dealloc {
	[_symbolValue release];
	_file = nil;
    [super dealloc];
}

- (NSString *)description {
	return [NSString stringWithFormat:@"name: %@ range: %@",[self name],NSStringFromRange([self symbolRange])];
}

- (BOOL)isLeaf {
	return ([self symbolType] != WCSymbolFileType);
}

@synthesize symbolType=_symbolType;
@synthesize symbolValue=_symbolValue;
@synthesize file=_file;
@synthesize symbolRange=_symbolRange;
@dynamic symbolsViewString;
- (NSString *)symbolsViewString {
	return [NSString stringWithFormat:NSLocalizedString(@"%@:%lu - %@", @"symbols view string"),[[[[self file] alias] absolutePathForDisplay] lastPathComponent],[[[self file] textStorage] lineNumberForCharacterIndex:[self symbolRange].location]+1,[self name]];
	
}

- (NSRange)jumpToRange {
	if ([self symbolType] == WCSymbolFileType)
		return NSMakeRange(0, 0);
	return [[[self file] textStorage] safeRangeForCharacterRange:[self symbolRange]];
}

- (WCFile *)jumpToFile {
	return [self file];
}

- (BOOL)shouldJumpToObject; {
	if ([self symbolType] == WCSymbolFileType)
		return NO;
	return YES;
}

- (NSString *)name {
	if ([self symbolType] == WCSymbolFileType)
		return [[self file] name];
	return [super name];
}

- (NSImage *)icon {
	if ([self symbolType] == WCSymbolFileType)
		return [[self file] icon];
	else if ([self symbolType] == WCSymbolLabelType)
		return [NSImage imageNamed:@"LabelSymbol"];
	else if ([self symbolType] == WCSymbolFunctionType)
		return [NSImage imageNamed:@"FunctionSymbol"];
	else if ([self symbolType] == WCSymbolEquateType)
		return [NSImage imageNamed:@"EquateSymbol"];
	else if ([self symbolType] == WCSymbolDefineType)
		return [NSImage imageNamed:@"DefineSymbol"];
	else if ([self symbolType] == WCSymbolMacroType)
		return [NSImage imageNamed:@"MacroSymbol"];
	return nil;
}

@dynamic iconForContextualMenu;
- (NSImage *)iconForContextualMenu {
	if ([self symbolType] == WCSymbolLabelType)
		return [NSImage imageNamed:@"LabelSymbol"];
	else if ([self symbolType] == WCSymbolFunctionType)
		return [NSImage imageNamed:@"FunctionSymbol"];
	else if ([self symbolType] == WCSymbolEquateType)
		return [NSImage imageNamed:@"EquateSymbol"];
	else if ([self symbolType] == WCSymbolDefineType)
		return [NSImage imageNamed:@"DefineSymbol"];
	else if ([self symbolType] == WCSymbolMacroType)
		return [NSImage imageNamed:@"MacroSymbol"];
	return nil;
}

@dynamic lineNumber;
- (NSUInteger)lineNumber {
	return [[[self file] textStorage] safeLineNumberForCharacterIndex:[self symbolRange].location];
}

+ (id)symbolWithName:(NSString *)name ofType:(WCSymbolType)type inFile:(WCFile *)file withRange:(NSRange)range; {
	return [[[[self class] alloc] initWithName:name type:type file:file range:range] autorelease];
}
- (id)initWithName:(NSString *)name type:(WCSymbolType)type file:(WCFile *)file range:(NSRange)range; {
	if (!(self = [super initWithName:name]))
		return nil;
	
	_symbolType = type;
	_file = file;
	_symbolRange = range;
	
	return self;
}

- (NSComparisonResult)compareUsingSymbolRange:(WCSymbol *)symbol; {
	NSRange mRange = [self symbolRange];
	NSRange range = [symbol symbolRange];
	
	if (mRange.location < range.location)
		return NSOrderedAscending;
	return NSOrderedDescending;
}
@end
