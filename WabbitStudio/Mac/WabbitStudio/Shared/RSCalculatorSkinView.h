//
//  RSCalculatorSkinView.h
//  WabbitStudio
//
//  Created by William Towe on 5/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSImageView.h>


@class RSCalculator;

@interface RSCalculatorSkinView : NSImageView {
@private
	RSCalculator *_calculator;
	BOOL _shouldMoveWindowOnMouseDragged;
}
@property (readonly,nonatomic) RSCalculator *calculator;

- (id)initWithCalculator:(RSCalculator *)calculator frame:(NSRect)frame;
@end
