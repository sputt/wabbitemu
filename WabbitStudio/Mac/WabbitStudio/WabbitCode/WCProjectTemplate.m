//
//  WCProjectTemplate.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectTemplate.h"


@implementation WCProjectTemplate

- (NSImage *)icon {
	if ([self isTemplateHeader])
		return [super icon];
	return [NSImage imageNamed:@"project.icns"];
}

@dynamic templateOutputType;
- (WCBuildTargetOutputType)templateOutputType {
	return [[[self templateDictionary] objectForKey:@"outputType"] unsignedIntegerValue];
}

@dynamic includeFileNames;
- (NSArray *)includeFileNames {
	return [[self templateDictionary] objectForKey:@"includeFiles"];
}

@end
