//
//  NSShadow+MCAdditions.m
//
//  Created by Sean Patrick O'Brien on 4/3/08.
//  Copyright 2008 MolokoCacao. All rights reserved.
//

#import "NSShadow+MCAdditions.h"


@implementation NSShadow (MCAdditions)

- (id)initWithColor:(NSColor *)color offset:(NSSize)offset blurRadius:(CGFloat)blur
{
	self = [self init];
	
	if (self != nil) {
		self.shadowColor = color;
		self.shadowOffset = offset;
		self.shadowBlurRadius = blur;
	}
	
	return self;
}

@end
