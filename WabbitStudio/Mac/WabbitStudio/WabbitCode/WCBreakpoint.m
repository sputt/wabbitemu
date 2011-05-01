//
//  WCBreakpoint.m
//  WabbitStudio
//
//  Created by William Towe on 4/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBreakpoint.h"
#import "WCFile.h"
#import "WCTextStorage.h"
#import "WCSymbol.h"
#import "WCSymbolScanner.h"
#import "NSArray+WCExtensions.h"
#import "WCGeneralPerformer.h"
#import "WCProject.h"

NSString *const kWCBreakpointIsActiveDidChangeNotification = @"kWCBreakpointIsActiveDidChangeNotification";

@implementation WCBreakpoint

+ (NSSet *)keyPathsForValuesAffectingName {
    return [NSSet setWithObjects:@"lineNumber", nil];
}
+ (NSSet *)keyPathsForValuesAffectingSymbolName {
    return [NSSet setWithObjects:@"lineNumber", nil];
}
+ (NSSet *)keyPathsForValuesAffectingAddress {
    return [NSSet setWithObjects:@"lineNumber", nil];
}
+ (NSSet *)keyPathsForValuesAffectingSymbolNameAndLineNumber {
    return [NSSet setWithObjects:@"symbolName",@"lineNumber", nil];
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_file = nil;
    [super dealloc];
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:[NSNumber numberWithUnsignedInteger:[self lineNumber]] forKey:@"lineNumber"];
	[coder encodeBool:[self isActive] forKey:@"isActive"];
	[super encodeWithCoder:coder];
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_lineNumber = [[coder decodeObjectForKey:@"lineNumber"] unsignedIntegerValue];
	_isActive = [coder decodeBoolForKey:@"isActive"];
	_breakpointType = WCBreakpointTypeLine;
	_isRam = YES;
	
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	WCBreakpoint *copy = [super copyWithZone:zone];
	
	copy->_file = _file;
	copy->_lineNumber = _lineNumber;
	copy->_isActive = _isActive;
	copy->_breakpointType = _breakpointType;
	copy->_isRam = _isRam;
	copy->_page = _page;
	copy->_address = _address;
	
	return copy;
}

- (NSDictionary *)plistRepresentation {
	NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:[super plistRepresentation]];
	
	[dict setObject:[NSNumber numberWithUnsignedInteger:[self lineNumber]] forKey:@"lineNumber"];
	[dict setObject:[NSNumber numberWithBool:[self isActive]] forKey:@"isActive"];
	[dict setObject:[NSNumber numberWithUnsignedInteger:[self breakpointType]] forKey:@"type"];
	
	return [[dict copy] autorelease];
}

- (NSRange)jumpToRange {
	return [self breakpointRange];
}

- (WCFile *)jumpToFile {
	return [self file];
}

- (BOOL)shouldJumpToObject; {
	if ([self breakpointType] != WCBreakpointTypeLine)
		return NO;
	return YES;
}

- (BOOL)isLeaf {
	if ([self breakpointType] == WCBreakpointTypeLine)
		return YES;
	return NO;
}

- (NSString *)name {
	switch ([self breakpointType]) {
		case WCBreakpointTypeLine: {
			if ([[super name] length])
				return [super name];
			
			NSString *string = [[[[[self file] textStorage] string] substringWithRange:[[[[self file] textStorage] string] lineRangeForRange:NSMakeRange([[[self file] textStorage] safeLineStartIndexForLineNumber:[self lineNumber]], 0)]] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
			
			if ([string length] == 0 || ([string length] == 1 && [[NSCharacterSet whitespaceAndNewlineCharacterSet] characterIsMember:[string characterAtIndex:0]]))
				return NSLocalizedString(@"Why did you put a breakpoint on an empty line?", @"Why did you put a breakpoint on an empty line?");
			return string;
		}
		case WCBreakpointTypeFile:
		case WCBreakpointTypeProject:
			return [[self file] name];
		default:
			return nil;
	}
}

- (NSImage *)icon {
	switch ([self breakpointType]) {
		case WCBreakpointTypeLine: {
			NSImage *retval = [[[NSImage alloc] initWithSize:NSMakeSize(24.0, 13.0)] autorelease];
			[retval setFlipped:YES];
			
			[retval lockFocus];
			[[WCGeneralPerformer sharedPerformer] drawBreakpoint:self inRect:NSMakeRect(0.0, 0.0, [retval size].width, [retval size].height)];
			[retval unlockFocus];
			
			return retval;
		}
		case WCBreakpointTypeFile:
		case WCBreakpointTypeProject:
			return [[self file] icon];
		default:
			return nil;
	}
}

+ (id)breakpointWithLineNumber:(NSUInteger)lineNumber inFile:(WCFile *)file; {
	return [[[[self class] alloc] initWithLineNumber:lineNumber file:file] autorelease];
}
- (id)initWithLineNumber:(NSUInteger)lineNumber file:(WCFile *)file; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_file = file;
	_lineNumber = lineNumber;
	_isActive = YES;
	_breakpointType = WCBreakpointTypeLine;
	_isRam = YES;
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileTextDidChange:) name:NSTextStorageDidProcessEditingNotification object:[_file textStorage]];
	
	return self;
}

@synthesize file=_file;
@synthesize lineNumber=_lineNumber;
@synthesize breakpointType=_breakpointType;
@dynamic isActive;
- (BOOL)isActive {
	return _isActive;
}
- (void)setIsActive:(BOOL)isActive {
	if (_isActive == isActive)
		return;
	
	_isActive = isActive;
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCBreakpointIsActiveDidChangeNotification object:self];
}
@dynamic breakpointRange;
- (NSRange)breakpointRange {
	if ([self breakpointType] == WCBreakpointTypeFile)
		return NSMakeRange(0, 0);
	return NSMakeRange([[[self file] textStorage] safeLineStartIndexForLineNumber:[self lineNumber]], 0);
}
@dynamic symbolName;
- (NSString *)symbolName {
	switch ([self breakpointType]) {
		case WCBreakpointTypeLine: {
			NSArray *symbols = [[[self file] symbolScanner] symbols];
			WCSymbol *symbol = [symbols objectAtIndex:[symbols symbolIndexForLocation:[[[self file] textStorage] safeLineStartIndexForLineNumber:[self lineNumber]]]];
			
			return [symbol name];
		}
		case WCBreakpointTypeFile:
		case WCBreakpointTypeProject:
			return [[self file] name];
		default:
			return nil;
	}
}
@dynamic symbolNameAndLineNumber;
- (NSString *)symbolNameAndLineNumber {
	if ([self breakpointType] == WCBreakpointTypeLine)
		return [NSString stringWithFormat:NSLocalizedString(@"%@ - line %lu", @"breakpoint symbol name and line number"),[self symbolName],[self lineNumber]+1];
	return [self name];
}
@dynamic symbolLineNumber;
- (NSUInteger)symbolLineNumber {
	switch ([self breakpointType]) {
		case WCBreakpointTypeLine: {
			NSArray *symbols = [[[self file] symbolScanner] symbols];
			WCSymbol *symbol = [symbols objectAtIndex:[symbols symbolIndexForLocation:[[[self file] textStorage] safeLineStartIndexForLineNumber:[self lineNumber]]]];
			
			return [symbol lineNumber];
		}
		case WCBreakpointTypeFile:
		case WCBreakpointTypeProject:
			return 0;
		default:
			return 0;
	}
}
@synthesize isRam=_isRam;
@synthesize page=_page;
@dynamic address;
- (u_int16_t)address {
	if ([[[self file] project] codeListing] == nil) {
#ifdef DEBUG
		NSLog(@"no listing file for project of %@",[[self file] name]);
#endif
		return _address;
	}
	
	NSString *codeListing = [[[self file] project] codeListing];
	// find where the listing for our file begins
	NSString *fileString = [NSString stringWithFormat:@"Listing for file \"%@\"",[[self file] absolutePathForDisplay]];
	NSRange fileRange = [codeListing rangeOfString:fileString options:NSLiteralSearch];
	
	if (fileRange.location == NSNotFound) {
#ifdef DEBUG
		NSLog(@"could not find listing for file %@",[[self file] name]);
#endif
		return _address;
	}
	
	// find the contents of our line within our file
	NSString *lineString = [[[[self file] textStorage] string] substringWithRange:[[[[self file] textStorage] string] lineRangeForRange:NSMakeRange([[[self file] textStorage] safeLineStartIndexForLineNumber:[self lineNumber]], 0)]];
	
	NSRange entireRange = NSMakeRange(0, [codeListing length]);
	NSRange searchRange = NSMakeRange(NSMaxRange(fileRange), [codeListing length]-NSMaxRange(fileRange));
	
	while (searchRange.location < entireRange.length) {
		NSRange lineRange = [codeListing rangeOfString:lineString options:NSLiteralSearch range:searchRange];
		
		if (lineRange.location == NSNotFound) {
#ifdef DEBUG
			NSLog(@"could not find listing for line %lu in file %@",[self lineNumber]+1,[[self file] name]);
#endif
			return _address;
		}
		
		// grab the entire matched line in the listing file
		NSString *listingLine = [codeListing substringWithRange:[codeListing lineRangeForRange:NSMakeRange(lineRange.location, 0)]];
		NSScanner *scanner = [NSScanner scannerWithString:listingLine];
		[scanner setCharactersToBeSkipped:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		NSInteger lineNumber;
		
		// grab the line number from the listing file
		if (![scanner scanInteger:&lineNumber]) {
#ifdef DEBUG
			NSLog(@"could not scan line number for line %lu in file %@",[self lineNumber]+1,[[self file] name]);
#endif
			return _address;
		}
		
		// if our line numbers are the same, we have the correct line, scan the rest of the line for page and address
		if (--lineNumber == [self lineNumber]) {
			// scan the page
			NSInteger page;
			
			if (![scanner scanInteger:&page]) {
#ifdef DEBUG
				NSLog(@"could not scan page for line %lu in file %@",[self lineNumber]+1,[[self file] name]);
#endif
				return _address;
			}
			
			[self setPage:(u_int8_t)page];
			
			// skip past the ':'
			[scanner setScanLocation:[scanner scanLocation]+1];
			
			unsigned newAddress;
			
			if (![scanner scanHexInt:&newAddress]) {
#ifdef DEBUG
				NSLog(@"could not scan address for line %lu in file %@",[self lineNumber]+1,[[self file] name]);
#endif
				return _address;
			}
			
#ifdef DEBUG
			NSLog(@"found page and address for line %lu in file %@",[self lineNumber]+1,[[self file] name]);
#endif
			_address = (u_int16_t)newAddress;
			break;
		}
		
#ifdef DEBUG
		NSLog(@"line numbers not equal advancing search range for file %@",[[self file] name]);
#endif
		// otherwise adjust the search range and try again
		searchRange = NSMakeRange(NSMaxRange(lineRange), entireRange.length-NSMaxRange(lineRange));
	}
	return _address;
}

- (void)_fileTextDidChange:(NSNotification *)note {
	[self willChangeValueForKey:@"name"];
	[self didChangeValueForKey:@"name"];
}
@end
