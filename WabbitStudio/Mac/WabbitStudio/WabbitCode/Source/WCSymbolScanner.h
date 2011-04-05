//
//  WCSymbolScanner.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>

@class WCFile,RKRegex;

extern NSString* const kWCSymbolScannerFinishedScanningNotification;
extern NSString* const kWCSymbolScannerFinishedScanningAfterSymbolsCaseSensitiveDidChangeNotification;

extern RKRegex *kWCSyntaxHighlighterMultilineCommentsRegex;

@interface WCSymbolScanner : NSObject {
@private
    WCFile *_file; // not retained
	BOOL _isScanning;
	
	NSSet *_labelStrings;
	NSSet *_equateStrings;
	NSSet *_defineStrings;
	NSSet *_macroStrings;
	
	NSArray *_symbols;
}
@property (readonly,nonatomic) WCFile *file;
@property (assign) BOOL isScanning;
@property (copy) NSSet *labelStrings;
@property (copy) NSSet *equateStrings;
@property (copy) NSSet *defineStrings;
@property (copy) NSSet *macroStrings;
@property (copy) NSArray *symbols;
@property (readonly,nonatomic) NSArray *equateSymbols;

- (id)initWithFile:(WCFile *)file;

- (void)scanSymbols;

- (NSArray *)symbolsForSymbolName:(NSString *)name;
@end
