//
//  WCSyntaxHighlighter.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@class WCTextView,RKRegex;

extern RKRegex *kWCSyntaxHighlighterDirectivesRegex;
extern RKRegex *kWCSyntaxHighlighterOpCodesRegex;
extern RKRegex *kWCSyntaxHighlighterConditionalsRegex;
extern RKRegex *kWCSyntaxHighlighterRegistersRegex;
extern RKRegex *kWCSyntaxHighlighterPreOpsRegex;
extern RKRegex *kWCSyntaxHighlighterStringsRegex;
extern RKRegex *kWCSyntaxHighlighterNumbersRegex;
extern RKRegex *kWCSyntaxHighlighterHexadeicmalsRegex;
extern RKRegex *kWCSyntaxHighlighterBinariesRegex;
extern RKRegex *kWCSyntaxHighlighterCommentsRegex;

extern RKRegex *kWCSyntaxHighlighterSymbolsRegex;

@interface WCSyntaxHighlighter : NSObject {
@private
    WCTextView *_textView; // not retained
}
- (id)initWithTextView:(WCTextView *)textView;

- (void)performSyntaxHighlighting;
@end
