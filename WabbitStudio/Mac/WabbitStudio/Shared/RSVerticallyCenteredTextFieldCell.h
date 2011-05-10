//
//  WCVerticallyCenteredTextFieldCell.h
//  WabbitStudio
//
//  Created by William Towe on 4/21/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTextFieldCell.h>


@interface RSVerticallyCenteredTextFieldCell : NSTextFieldCell <NSCopying> {
@private
	BOOL _excludeFileExtensionWhenSelecting;
}
@property (assign,nonatomic) BOOL excludeFileExtensionWhenSelecting;

- (NSRect)centeredTitleRectForBounds:(NSRect)bounds;

- (void)commonInit;
@end
