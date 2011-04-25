//
//  WCFindInProjectResult.h
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"
#import "WCJumpToObjectProtocol.h"

@class WCFile,WCSymbol;

@interface WCFindInProjectResult : WCTreeNode <WCJumpToObject> {
@private
	__weak WCFile *_file;
	__weak WCSymbol *_symbol;
	NSRange _findRange; // the matching range
	NSAttributedString *_findString; // string containing the entire line(s) that the match is contained in
}
@property (readonly,nonatomic) WCFile *file;
@property (readonly,nonatomic) NSRange findRange;
@property (readonly,nonatomic) NSAttributedString *findString;
@property (readonly,nonatomic) WCSymbol *symbol;

+ (id)findInProjectResultForFile:(WCFile *)file inRange:(NSRange)range withString:(NSAttributedString *)string symbol:(WCSymbol *)symbol;
- (id)initWithFile:(WCFile *)file range:(NSRange)range string:(NSAttributedString *)string symbol:(WCSymbol *)symbol;
@end
