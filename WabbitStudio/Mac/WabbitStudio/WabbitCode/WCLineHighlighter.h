//
//  WCLineHighlighter.h
//  WabbitStudio
//
//  Created by William Towe on 3/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#import <AppKit/NSTextView.h>


@interface WCLineHighlighter : NSObject {
@private
    __weak NSTextView *_textView;
}
- (id)initWithTextView:(NSTextView *)textView;
@end
