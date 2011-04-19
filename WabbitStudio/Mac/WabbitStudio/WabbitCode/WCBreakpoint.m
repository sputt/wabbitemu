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

NSString *const kWCBreakpointIsActiveDidChangeNotification = @"kWCBreakpointIsActiveDidChangeNotification";

@implementation WCBreakpoint

- (void)dealloc {
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
	
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	WCBreakpoint *copy = [super copyWithZone:zone];
	
	copy->_lineNumber = _lineNumber;
	copy->_isActive = _isActive;
	copy->_breakpointType = _breakpointType;
	
	return copy;
}

- (BOOL)isLeaf {
	if ([self breakpointType] == WCBreakpointTypeLine)
		return YES;
	return NO;
}

- (NSString *)name {
	switch ([self breakpointType]) {
		case WCBreakpointTypeLine: {
			NSArray *symbols = [[[self file] symbolScanner] symbols];
			WCSymbol *symbol = [symbols objectAtIndex:[symbols symbolIndexForLocation:[[[self file] textStorage] lineStartIndexForLineNumber:[self lineNumber]]]];
			
			return [NSString stringWithFormat:NSLocalizedString(@"%@ - line %lu", @"breakpoint line name"),[symbol name],[self lineNumber]+1];
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
			NSImage *retval = [[[NSImage alloc] initWithSize:NSMakeSize(32.0, 14.0)] autorelease];
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
	return NSMakeRange([[[self file] textStorage] lineStartIndexForLineNumber:[self lineNumber]], 0);
}
@end
