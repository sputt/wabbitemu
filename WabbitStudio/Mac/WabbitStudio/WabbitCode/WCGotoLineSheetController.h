//
//  WCGotoLineSheetController.h
//  WabbitStudio
//
//  Created by William Towe on 4/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>

enum WCGotoLineOrCharacter {
	WCGotoLineOrCharacterLine = 0,
	WCGotoLineOrCharacterCharacter
};
typedef NSUInteger WCGotoLineOrCharacter;

@class WCTextView;

@interface WCGotoLineSheetController : NSWindowController {
@private
    NSNumber *_lineNumber;
	WCGotoLineOrCharacter _lineOrCharacter;
	__weak WCTextView *_textView;
}
@property (copy,nonatomic) NSNumber *lineNumber;
@property (assign,nonatomic) WCGotoLineOrCharacter lineOrCharacter;

+ (void)presentGotoLineSheetForTextView:(WCTextView *)textView;
@end
