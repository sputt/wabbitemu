//
//  WCBreakpoint.h
//  WabbitStudio
//
//  Created by William Towe on 4/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"
#import "WCJumpToObjectProtocol.h"

enum {
	WCBreakpointTypeProject = 0,
	WCBreakpointTypeFile = 1,
	WCBreakpointTypeLine = 2
};
typedef NSUInteger WCBreakpointType;

extern NSString *const kWCBreakpointIsActiveDidChangeNotification;

@class WCFile;

@interface WCBreakpoint : WCTreeNode <NSCoding,NSCopying,WCPlistRepresentation,WCJumpToObject> {
@private
	__weak WCFile *_file;
    NSUInteger _lineNumber;
	BOOL _isActive;
	WCBreakpointType _breakpointType;
	
	u_int8_t _page;
	u_int16_t _address;
	BOOL _isRam;
}
@property (assign,nonatomic) WCFile *file;
@property (assign,nonatomic) NSUInteger lineNumber;
@property (assign,nonatomic) BOOL isActive;
@property (assign,nonatomic) WCBreakpointType breakpointType;
@property (readonly,nonatomic) NSRange breakpointRange;
@property (assign,nonatomic) BOOL isRam;
@property (assign,nonatomic) u_int8_t page;
@property (readonly,nonatomic) u_int16_t address;
@property (readonly,nonatomic) NSString *symbolName;
@property (readonly,nonatomic) NSString *symbolNameAndLineNumber;

+ (id)breakpointWithLineNumber:(NSUInteger)lineNumber inFile:(WCFile *)file;
- (id)initWithLineNumber:(NSUInteger)lineNumber file:(WCFile *)file;
@end
