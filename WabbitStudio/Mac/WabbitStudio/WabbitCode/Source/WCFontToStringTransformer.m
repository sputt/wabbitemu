//
//  WCFontToStringTransformer.m
//  WabbitStudio
//
//  Created by William Towe on 3/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFontToStringTransformer.h"


@implementation WCFontToStringTransformer

+ (BOOL)allowsReverseTransformation {
	return NO;
}
+ (Class)transformedValueClass {
	return [NSString class];
}
- (id)transformedValue:(id)value {
	NSFont *font = [NSKeyedUnarchiver unarchiveObjectWithData:value];
	
	return [NSString stringWithFormat:@"%@ - %.1f",[font fontName],[font pointSize]];
}

@end
