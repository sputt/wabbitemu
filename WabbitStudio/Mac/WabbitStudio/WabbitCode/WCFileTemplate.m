//
//  WCFileTemplate.m
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFileTemplate.h"

@implementation WCFileTemplate

- (void)dealloc {
	[_fileName release];
    [super dealloc];
}

- (NSImage *)icon {
	if ([self isTemplateHeader])
		return [super icon];
	return [[NSWorkspace sharedWorkspace] iconForFile:[[[self templateURL] URLByAppendingPathComponent:[self templateFileName]] path]];
}


@dynamic templateFileName;
- (NSString *)templateFileName {
	return [[self templateDictionary] objectForKey:@"templateFileName"];
}
@synthesize fileName=_fileName;



@end
