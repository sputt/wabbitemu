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
    __weak WCFile *_file; // not retained
	BOOL _isScanning;
	
	NSDictionary *_labelNamesToSymbols;
	NSDictionary *_equateNamesToSymbols;
	NSDictionary *_defineNamesToSymbols;
	NSDictionary *_macroNamesToSymbols;
	
	NSArray *_symbols; // this is always sorted by location in the text, from top to bottom
}
@property (readonly,nonatomic) WCFile *file;
@property (assign,nonatomic) BOOL isScanning;
@property (copy) NSArray *symbols;
@property (copy) NSDictionary *labelNamesToSymbols;
@property (copy) NSDictionary *equateNamesToSymbols;
@property (copy) NSDictionary *defineNamesToSymbols;
@property (copy) NSDictionary *macroNamesToSymbols;

- (id)initWithFile:(WCFile *)file;

- (void)scanSymbols;

- (NSArray *)symbolsForSymbolName:(NSString *)name;
@end
