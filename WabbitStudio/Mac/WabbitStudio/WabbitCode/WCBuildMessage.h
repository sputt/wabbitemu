//
//  WCBuildMessage.h
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"
#import "WCJumpToObjectProtocol.h"

enum WCBuildMessageType {
	WCBuildMessageTypeFile = 0,
	WCBuildMessageTypeError,
	WCBuildMessageTypeWarning
};
typedef NSUInteger WCBuildMessageType;

@class WCFile;

@interface WCBuildMessage : WCTreeNode <WCJumpToObject> {
@private
	__weak WCFile *_file;
    WCBuildMessageType _messageType;
	NSUInteger _lineNumber;
}
@property (readonly,nonatomic) WCBuildMessageType messageType;
@property (readonly,nonatomic) WCFile *file;
@property (readonly,nonatomic) NSUInteger lineNumber;
@property (readonly,nonatomic) NSUInteger messagesCount;
@property (readonly,nonatomic) NSUInteger errorsCount;
@property (readonly,nonatomic) NSUInteger warningsCount;

+ (id)buildMessageWithMessage:(NSString *)message ofType:(WCBuildMessageType)type inFile:(WCFile *)file atLineNumber:(NSUInteger)lineNumber;
- (id)initWithMessage:(NSString *)message type:(WCBuildMessageType)type file:(WCFile *)file lineNumber:(NSUInteger)lineNumber;
@end
