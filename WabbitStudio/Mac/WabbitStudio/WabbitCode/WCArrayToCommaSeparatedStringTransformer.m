//
//  WCArrayToCommaSeparatedStringTransformer.m
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCArrayToCommaSeparatedStringTransformer.h"
#import "WCDefines.h"


@implementation WCArrayToCommaSeparatedStringTransformer

+ (BOOL)allowsReverseTransformation {
	return NO;
}
+ (Class)transformedValueClass {
	return [NSString class];
}
- (id)transformedValue:(id)value {	
	NSArray *array = (NSArray *)value;
	
	if ([array count] == 0)
		return NS_LOCALIZED_STRING_NONE;
	return [array componentsJoinedByString:@", "];
}

@end
