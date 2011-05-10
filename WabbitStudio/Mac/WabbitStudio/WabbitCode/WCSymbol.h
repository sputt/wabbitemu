//
//  WCSymbol.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"
#import "WCJumpToObjectProtocol.h"


enum WCSymbolType {
	WCSymbolFileType = 0,
	WCSymbolLabelType,
	WCSymbolEquateType,
	WCSymbolDefineType,
	WCSymbolMacroType,
	WCSymbolFunctionType
};
typedef NSUInteger WCSymbolType;

@class WCFile;

@interface WCSymbol : WCTreeNode <WCJumpToObject> {
@private
	__weak WCFile *_file;
    WCSymbolType _symbolType;
	NSString *_symbolValue;
	NSRange _symbolRange;
}
@property (assign,nonatomic) WCSymbolType symbolType;
@property (copy) NSString *symbolValue;
@property (assign,nonatomic) WCFile *file;
@property (assign) NSRange symbolRange;
@property (readonly,nonatomic) NSImage *iconForContextualMenu;
@property (readonly,nonatomic) NSString *symbolsViewString;
@property (readonly,nonatomic) NSUInteger lineNumber;

+ (id)symbolWithName:(NSString *)name ofType:(WCSymbolType)type inFile:(WCFile *)file withRange:(NSRange)range;
- (id)initWithName:(NSString *)name type:(WCSymbolType)type file:(WCFile *)file range:(NSRange)range;

- (NSComparisonResult)compareUsingSymbolRange:(WCSymbol *)symbol;
@end
