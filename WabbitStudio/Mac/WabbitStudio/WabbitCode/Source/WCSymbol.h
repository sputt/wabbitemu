//
//  WCSymbol.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"


enum WCSymbolType {
	WCSymbolFileType = 0,
	WCSymbolLabelType,
	WCSymbolEquateType,
	WCSymbolDefineType,
	WCSymbolMacroType
};
typedef NSUInteger WCSymbolType;

@class WCFile;

@interface WCSymbol : WCTreeNode {
@private
    WCSymbolType _symbolType;
	NSString *_symbolValue;
	WCFile *_file; // not retained
	NSRange _symbolRange;
}
@property (assign,nonatomic) WCSymbolType symbolType;
@property (copy) NSString *symbolValue;
@property (assign,nonatomic) WCFile *file;
@property (assign) NSRange symbolRange;
@property (readonly,nonatomic) NSImage *iconForContextualMenu;

+ (id)symbolWithName:(NSString *)name ofType:(WCSymbolType)type inFile:(WCFile *)file withRange:(NSRange)range;
- (id)initWithName:(NSString *)name type:(WCSymbolType)type file:(WCFile *)file range:(NSRange)range;

- (NSComparisonResult)compareUsingSymbolRange:(WCSymbol *)symbol;
@end
