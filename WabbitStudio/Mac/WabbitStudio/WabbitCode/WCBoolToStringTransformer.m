//
//  WCBoolToStringTransformer.m
//  WabbitStudio
//
//  Created by William Towe on 4/21/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBoolToStringTransformer.h"
#import "WCDefines.h"


@implementation WCBoolToStringTransformer

+ (BOOL)allowsReverseTransformation {
	return NO;
}

+ (Class)transformedValueClass {
	return [NSString class];
}

- (id)transformedValue:(id)value {
	if (value == nil)
		return NS_LOCALIZED_STRING_FALSE;
	return ([value boolValue])?NS_LOCALIZED_STRING_TRUE:NS_LOCALIZED_STRING_FALSE;
}
@end
