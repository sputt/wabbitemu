//
//  WCBreakpoint.m
//  WabbitStudio
//
//  Created by William Towe on 4/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBreakpoint.h"
#import "WCFile.h"

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
	
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	WCBreakpoint *copy = [super copyWithZone:zone];
	
	copy->_lineNumber = _lineNumber;
	copy->_isActive = _isActive;
	
	return copy;
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
	
	return self;
}

@synthesize file=_file;
@synthesize lineNumber=_lineNumber;
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
@end
