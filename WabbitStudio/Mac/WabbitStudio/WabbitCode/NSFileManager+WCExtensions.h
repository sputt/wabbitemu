//
//  NSFileManager+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSFileManager.h>


@interface NSFileManager (NSFileManager_WCExtensions)
- (BOOL)directoryExistsAtURL:(NSURL *)url;
- (BOOL)directoryExistsAtPath:(NSString *)path;
- (BOOL)fileExistsAtURL:(NSURL *)url;

- (NSString *)createDirectoryAtPath:(NSString *)path error:(NSError **)error;

- (NSArray *)copyItemsInDirectory:(NSURL *)srcURL ofTypes:(NSArray *)typeUTIs toURL:(NSURL *)destURL error:(NSError **)error;

- (NSString *)uniqueFilePathForPath:(NSString *)path;
@end
