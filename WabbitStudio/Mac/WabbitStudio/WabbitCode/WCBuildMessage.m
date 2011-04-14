//
//  WCBuildMessage.m
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildMessage.h"
#import "WCFile.h"
#import "NSImage+WCExtensions.h"


@implementation WCBuildMessage

- (void)dealloc {
	_file = nil;
    [super dealloc];
}

- (NSString *)name {
	if ([self messageType] == WCBuildMessageTypeFile)
		return [[self file] name];
	return [super name];
}

- (NSImage *)icon {
	switch (_messageType) {
		case WCBuildMessageTypeFile:
			return [_file icon];
		case WCBuildMessageTypeError:
			return [NSImage imageNamed:@"ErrorBadge"];
		case WCBuildMessageTypeWarning:
			return [NSImage imageNamed:@"WarningBadge"];
		default:
			return nil;
	}
}

- (BOOL)isLeaf {
	switch ([self messageType]) {
		case WCBuildMessageTypeFile:
			return NO;
		default:
			return YES;
	}
}
@synthesize messageType=_messageType;
@synthesize file=_file;
@synthesize lineNumber=_lineNumber;
@dynamic messagesCount;
- (NSUInteger)messagesCount {
	if ([self messageType] == WCBuildMessageTypeFile)
		return [[self childNodes] count];
	return 0;
}
@dynamic errorsCount;
- (NSUInteger)errorsCount {
	if ([self messageType] != WCBuildMessageTypeFile)
		return 0;
	NSUInteger retval = 0;
	for (WCBuildMessage *m in [self childNodes]) {
		if ([m messageType] == WCBuildMessageTypeError)
			retval++;
	}
	return retval;
}
@dynamic warningsCount;
- (NSUInteger)warningsCount {
	if ([self messageType] != WCBuildMessageTypeFile)
		return 0;
	NSUInteger retval = 0;
	for (WCBuildMessage *m in [self childNodes]) {
		if ([m messageType] == WCBuildMessageTypeWarning)
			retval++;
	}
	return retval;
}

+ (id)buildMessageWithMessage:(NSString *)message ofType:(WCBuildMessageType)type inFile:(WCFile *)file atLineNumber:(NSUInteger)lineNumber; {
	return [[[[self class] alloc] initWithMessage:message type:type file:file lineNumber:lineNumber] autorelease];
}
- (id)initWithMessage:(NSString *)message type:(WCBuildMessageType)type file:(WCFile *)file lineNumber:(NSUInteger)lineNumber; {
	if (!(self = [super initWithName:message]))
		return nil;
	
	_messageType = type;
	_file = file;
	_lineNumber = lineNumber;
	
	return self;
}
@end
