//
//  WCLineHighlighter.h
//  WabbitStudio
//
//  Created by William Towe on 3/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@interface WCLineHighlighter : NSObject {
@private
    NSTextView *_textView; // not retained
}
- (id)initWithTextView:(NSTextView *)textView;
@end
