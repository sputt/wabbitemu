//
//  WCGradientTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCDoEverythingTextFieldCell.h"


@interface WCGradientTextFieldCell : WCDoEverythingTextFieldCell {
@private
	BOOL _drawGradient; // our corresponding NSTableView class actually draws the gradient, this just tells it whether to draw
}
@property (assign,nonatomic) BOOL drawGradient;
@property (readonly,nonatomic) NSGradient *gradient;
@property (readonly,nonatomic) NSColor *borderColor;
@end
