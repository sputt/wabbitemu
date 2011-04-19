//
//  WCBreakpoint.h
//  WabbitStudio
//
//  Created by William Towe on 4/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"

extern NSString *const kWCBreakpointIsActiveDidChangeNotification;

@class WCFile;

@interface WCBreakpoint : WCTreeNode <NSCoding,NSCopying> {
@private
	__weak WCFile *_file;
    NSUInteger _lineNumber;
	BOOL _isActive;
}
@property (assign,nonatomic) WCFile *file;
@property (assign,nonatomic) NSUInteger lineNumber;
@property (assign,nonatomic) BOOL isActive;

+ (id)breakpointWithLineNumber:(NSUInteger)lineNumber inFile:(WCFile *)file;
- (id)initWithLineNumber:(NSUInteger)lineNumber file:(WCFile *)file;
@end
