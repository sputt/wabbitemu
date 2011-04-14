//
//  WCBadgedTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCIconTextFieldCell.h"


@interface WCBadgedTextFieldCell : WCIconTextFieldCell {
@private
    NSUInteger _badgeCount;
}
@property (assign,nonatomic) NSUInteger badgeCount;
@end
