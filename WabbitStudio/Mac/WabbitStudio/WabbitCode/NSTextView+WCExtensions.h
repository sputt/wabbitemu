//
//  NSTextView+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 4/6/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface NSTextView (NSTextView_WCExtensions)
// both of these methods assume the replacementRanges array is sorted in ascending order
- (NSArray *)replaceCharactersInRanges:(NSArray *)replacementRanges withString:(NSString *)string;
- (NSArray *)replaceCharactersInRanges:(NSArray *)replacementRanges withStrings:(NSArray *)strings;

- (void)setSelectedRangeSafely:(NSRange)range;
- (void)setSelectedRangeSafely:(NSRange)range scrollRangeToVisible:(BOOL)scrollRangeToVisible;
@end
