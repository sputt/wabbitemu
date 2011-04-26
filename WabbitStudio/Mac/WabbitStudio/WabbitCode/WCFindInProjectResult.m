//
//  WCFindInProjectResult.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindInProjectResult.h"
#import "WCFile.h"
#import "WCSymbol.h"
#import "WCTextStorage.h"
#import "WCProject.h"
#import "WCFindInProjectViewController.h"


@implementation WCFindInProjectResult

- (void)dealloc {
	_symbol = nil;
	_file = nil;
	[_findString release];
    [super dealloc];
}

- (NSString *)name {
	if (![self parentNode])
		return [[self file] name];
	return [[self findString] string];
}

- (NSImage *)icon {
	if ([self parentNode] == nil)
		return [[self file] icon];
	else if ([self symbol] != nil)
		return [[self symbol] icon];
	return [NSImage imageNamed:@"FindInProjectResult16x16"];
}

- (NSRange)jumpToRange {
	return [[[self file] textStorage] safeRangeForCharacterRange:[self findRange]];
}

- (WCFile *)jumpToFile {
	return [self file];
}
- (BOOL)shouldJumpToObject; {
	if ([self parentNode] == nil)
		return NO;
	return YES;
}

@synthesize file=_file;
@synthesize symbol=_symbol;
@synthesize findRange=_findRange;
@synthesize findString=_findString;
@dynamic lineNumber;
- (NSUInteger)lineNumber {
	return [[[self file] textStorage] safeLineNumberForCharacterIndex:[self findRange].location];
}

+ (id)findInProjectResultForFile:(WCFile *)file inRange:(NSRange)range withString:(NSAttributedString *)string symbol:(WCSymbol *)symbol; {
	return [[[[self class] alloc] initWithFile:file range:range string:string symbol:symbol] autorelease];
}
- (id)initWithFile:(WCFile *)file range:(NSRange)range string:(NSAttributedString *)string symbol:(WCSymbol *)symbol; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_file = file; 
	_symbol = symbol;
	_findRange = range;
	_findString = [string copy];
	
	return self;
}
@end
