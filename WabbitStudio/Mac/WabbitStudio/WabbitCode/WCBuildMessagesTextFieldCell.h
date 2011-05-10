//
//  WCBuildMessagesTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 5/8/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSBadgedTextFieldCell.h"


@interface WCBuildMessagesTextFieldCell : RSBadgedTextFieldCell {
@private
    BOOL _hasOnlyWarnings;
}
@property (assign,nonatomic) BOOL hasOnlyWarnings;
@end
