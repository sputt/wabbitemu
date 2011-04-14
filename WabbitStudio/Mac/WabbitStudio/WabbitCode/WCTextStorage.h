//
//  WCTextStorage.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTextStorage.h>


@interface WCTextStorage : NSTextStorage {
@private
	NSMutableAttributedString *_contents; // our actual storage
	NSMutableArray *_lineStartIndexes; // stores the starting character index of every line in the text
}
- (NSArray *)lineStartIndexes;
- (NSUInteger)numberOfLines;
- (NSUInteger)lineNumberForCharacterIndex:(NSUInteger)index;
- (NSUInteger)lineStartIndexForLineNumber:(NSUInteger)line;
- (NSUInteger)lineStartIndexForCharacterIndex:(NSUInteger)index;
//- (NSRange)lineRangeForCharacterIndex:(NSUInteger)index;
@end
