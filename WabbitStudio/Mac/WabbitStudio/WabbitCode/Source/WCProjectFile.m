//
//  WCProjectFile.m
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectFile.h"
#import "WCProject.h"

@implementation WCProjectFile
#pragma mark *** Subclass Overrides ***
- (NSString *)name {
	return [[self project] displayName];
}

- (NSImage *)icon {
	return [NSImage imageNamed:@"project.icns"];
}

- (NSString *)filePath {
	return [[[self project] fileURL] path];
}

- (NSURL *)directoryURL {
	return [[[self project] fileURL] URLByDeletingLastPathComponent];
}

- (BOOL)isDirectory {
	return YES;
}

- (BOOL)canEditName {
	return NO;
}
@end
