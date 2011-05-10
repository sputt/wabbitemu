//
//  WCMultiLineTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 4/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSVerticallyCenteredTextFieldCell.h"


@class WCInfoTextFieldCell;

@interface RSMultiLineTextFieldCell : RSVerticallyCenteredTextFieldCell <NSCopying> {
@private
	WCInfoTextFieldCell *_secondaryTextFieldCell;
}
@property (copy,nonatomic) NSString *secondaryTitle;

- (NSRect)secondaryTitleRectForBounds:(NSRect)bounds remainingRect:(NSRectPointer)remainingRect;
@end
