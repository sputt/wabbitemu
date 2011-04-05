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
	BOOL _drawGradient;
}
@property (assign,nonatomic) BOOL drawGradient;
@end
