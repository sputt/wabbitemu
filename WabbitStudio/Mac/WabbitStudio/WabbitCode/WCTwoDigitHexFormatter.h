//
//  WCTwoDigitHexFormatter.h
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCHexFormatter.h"


@interface WCTwoDigitHexFormatter : WCHexFormatter {
@private
    BOOL _shouldDrawWithProgramCounterAttributes;
	BOOL _cellIsHighlighted;
}
@property (assign,nonatomic) BOOL shouldDrawWithProgramCounterAttributes;
@property (assign,nonatomic) BOOL cellIsHighlighted;
@end
