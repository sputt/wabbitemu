//
//  NSFileManager+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSFileManager+WCExtensions.h"


@implementation NSFileManager (NSFileManager_WCExtensions)
// same as NSFileManager directoryExistsAtPath: but with an NSURL
- (BOOL)directoryExistsAtURL:(NSURL *)url; {
	return [self directoryExistsAtPath:[url path]];
}
- (BOOL)directoryExistsAtPath:(NSString *)path; {
	BOOL isDirectory = NO;
	return ([self fileExistsAtPath:path isDirectory:&isDirectory] && isDirectory);
}
- (BOOL)fileExistsAtURL:(NSURL *)url; {
	return [self fileExistsAtPath:[url path]];
}

- (NSString *)createDirectoryAtPath:(NSString *)path error:(NSError **)error; {
	if ([self directoryExistsAtPath:path])
		return path;

	if (![self createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:error])
		return nil;
	return path;
}

// copies items in directory 'srcURL' of a type contained in 'typeUTIs' to 'destURL'
// returns an array of the URLs that were copied
- (NSArray *)copyItemsInDirectory:(NSURL *)srcURL ofTypes:(NSArray *)typeUTIs toURL:(NSURL *)destURL error:(NSError **)error; {
	NSMutableArray *retval = [NSMutableArray array];
	for (NSURL *url in [self contentsOfDirectoryAtURL:srcURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
		if (![typeUTIs containsObject:[[NSWorkspace sharedWorkspace] typeOfFile:[url path] error:NULL]])
			continue;
		
		if (![self copyItemAtURL:url toURL:[destURL URLByAppendingPathComponent:[url lastPathComponent]] error:NULL])
			continue;
		
		[retval addObject:[destURL URLByAppendingPathComponent:[url lastPathComponent]]];
	}
	return [[retval copy] autorelease];
}

- (NSString *)uniqueFilePathForPath:(NSString *)path; {
	if (![self fileExistsAtPath:path])
		return path;
	
	NSUInteger suffix = 1;
	NSString *directory = [path stringByDeletingLastPathComponent];
	NSString *name = [[path lastPathComponent] stringByDeletingPathExtension];
	NSString *extension = [path pathExtension];
	NSString *retval = [directory stringByAppendingPathComponent:[[NSString stringWithFormat:@"%@%u",name,suffix] stringByAppendingPathExtension:extension]];
	
	while ([self fileExistsAtPath:retval])
		retval = [directory stringByAppendingPathComponent:[[NSString stringWithFormat:@"%@%u",name,++suffix] stringByAppendingPathExtension:extension]];
	
	return retval;
}
@end
