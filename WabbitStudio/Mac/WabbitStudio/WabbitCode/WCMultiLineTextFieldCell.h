//
//  WCMultiLineTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 4/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCVerticallyCenteredTextFieldCell.h"


@interface WCMultiLineTextFieldCell : WCVerticallyCenteredTextFieldCell <NSCopying> {
@private
    NSString *_secondaryTitle;
}
@property (copy,nonatomic) NSString *secondaryTitle;

- (NSRect)secondaryTitleRectForBounds:(NSRect)bounds;
@end
