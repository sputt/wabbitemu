//
//  WCBuildMessagesTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 5/8/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBadgedTextFieldCell.h"


@interface WCBuildMessagesTextFieldCell : WCBadgedTextFieldCell {
@private
    BOOL _hasOnlyWarnings;
}
@property (assign,nonatomic) BOOL hasOnlyWarnings;
@end
