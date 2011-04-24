//
//  WCGradientTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCIconTextFieldCell.h"


@interface WCGradientTextFieldCell : WCIconTextFieldCell {
@private
	BOOL _drawGradient; // our corresponding NSTableView class actually draws the gradient
}
@property (assign,nonatomic) BOOL drawGradient;
@property (readonly,nonatomic) NSGradient *gradient;
@property (readonly,nonatomic) NSColor *borderColor;
@end
