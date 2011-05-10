//
//  WCGradientView.h
//  WabbitStudio
//
//  Created by William Towe on 4/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface WCGradientView : NSView {
@private
    NSGradient *_gradient;
	NSColor *_topLineColor;
	NSColor *_bottomLineColor;
	BOOL _drawsTopLine;
	BOOL _drawsBottomLine;
	CGFloat _angle;
}
@property (retain,nonatomic) NSGradient *gradient;
@property (assign,nonatomic) CGFloat angle;
@property (retain,nonatomic) NSColor *topLineColor;
@property (retain,nonatomic) NSColor *bottomLineColor;
@property (assign,nonatomic) BOOL drawsTopLine;
@property (assign,nonatomic) BOOL drawsBottomLine;

- (void)commonInit;
@end
