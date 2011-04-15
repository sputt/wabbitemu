//
//  WCVerticallyCenteredTextFieldCell.m
//  WabbitCode
//
//  Created by William Towe on 10/17/08.
//  Copyright 2008 Revolution Software. All rights reserved.
//

#import "WCVerticallyCenteredTextFieldCell.h"


@interface WCVerticallyCenteredTextFieldCell (Private)
- (void)_setVerticallyCentered:(BOOL)flag;
@end

@implementation WCVerticallyCenteredTextFieldCell
#pragma mark *** Subclass Overrides ***
- (id)initTextCell:(NSString *)string {
	if (!(self = [super initTextCell:string]))
		return nil;
	
	// this is a hack to get vertically centered text
	if ([self respondsToSelector:@selector(_setVerticallyCentered:)])
		[self _setVerticallyCentered:YES];
	
	return self;
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	// this is a hack to get vertically centered text
	if ([self respondsToSelector:@selector(_setVerticallyCentered:)])
		[self _setVerticallyCentered:YES];
	
	return self;
}
@end
