//
//  WCProjectTemplate.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectTemplate.h"

NSString *const kWCProjectTemplateOutputTypeKey = @"outputType";
NSString *const kWCProjectTemplateDescriptionKey = @"description";
NSString *const kWCProjectTemplateIncludeFileNamesKey = @"includeFileNames";

@implementation WCProjectTemplate

- (void)dealloc {
	[_projectName release];
	[_templateURL release];
	[_templateDictionary release];
    [super dealloc];
}

- (NSString *)name {
	if ([self isTemplateHeader])
		return [super name];
	return [[self templateURL] lastPathComponent];
}

- (NSImage *)icon {
	if ([self isTemplateHeader])
		return [super icon];
	return [NSImage imageNamed:@"project.icns"];
}

@synthesize templateURL=_templateURL;
@dynamic templateDictionary;
- (NSDictionary *)templateDictionary {
	if (!_templateDictionary) {
		if ([[[self templateURL] URLByAppendingPathComponent:@"info.plist"] checkResourceIsReachableAndReturnError:NULL])
			_templateDictionary = [[NSDictionary alloc] initWithContentsOfURL:[[self templateURL] URLByAppendingPathComponent:@"info.plist"]];
	}
	return _templateDictionary;
}
@dynamic templateOutputType;
- (WCBuildTargetOutputType)templateOutputType {
	return [[[self templateDictionary] objectForKey:kWCProjectTemplateOutputTypeKey] unsignedIntegerValue];
}
@dynamic templateDescription;
- (NSString *)templateDescription {
	return [[self templateDictionary] objectForKey:kWCProjectTemplateDescriptionKey];
}
@dynamic includeFileNames;
- (NSArray *)includeFileNames {
	return [[self templateDictionary] objectForKey:kWCProjectTemplateIncludeFileNamesKey];
}
@dynamic isTemplateHeader;
- (BOOL)isTemplateHeader {
	return ([self templateURL] == nil); 
}
@synthesize projectName=_projectName;

+ (id)templateWithURL:(NSURL *)URL; {
	return [[[[self class] alloc] initWithURL:URL] autorelease];
}
- (id)initWithURL:(NSURL *)URL; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_templateURL = [URL retain];
	
	return self;
}
@end
