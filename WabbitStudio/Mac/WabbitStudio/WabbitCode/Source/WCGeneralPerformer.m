//
//  WCGeneralPerformer.m
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGeneralPerformer.h"
#import "WCFile.h"
#import "NSFileManager+WCExtensions.h"
#import "WCProject.h"
#import "WCProjectFile.h"
#import "WCPreferencesController.h"
#import "WCBuildTarget.h"
#import "WCProjectTemplate.h"
#import "NSString+WCExtensions.h"
#import "WCFileTemplate.h"


@implementation WCGeneralPerformer

- (BOOL)addFileURLsInDirectory:(NSURL *)directoryURL toFile:(WCFile *)file; {
	return [self addFileURLsInDirectory:directoryURL toFile:file inDirectoryURL:nil atIndex:0];
}
- (BOOL)addFileURLsInDirectory:(NSURL *)directoryURL toFile:(WCFile *)file inDirectoryURL:(NSURL *)directory; {
	return [self addFileURLsInDirectory:directoryURL toFile:file inDirectoryURL:directory atIndex:0];
}
- (BOOL)addFileURLsInDirectory:(NSURL *)directoryURL toFile:(WCFile *)file inDirectoryURL:(NSURL *)directory atIndex:(NSUInteger)index; {
	return [self addFileURLs:[[NSFileManager defaultManager] contentsOfDirectoryAtURL:directoryURL includingPropertiesForKeys:nil options:0 error:NULL] toFile:file inDirectoryURL:directory atIndex:index];
}

- (BOOL)addFileURLs:(NSArray *)urls toFile:(WCFile *)file; {
	return [self addFileURLs:urls toFile:file atIndex:0];
}

- (BOOL)addFileURLs:(NSArray *)urls toFile:(WCFile *)file atIndex:(NSUInteger)index; {
	return [self addFileURLs:urls toFile:file inDirectoryURL:nil atIndex:index];
}

- (BOOL)addFileURLs:(NSArray *)urls toFile:(WCFile *)file inDirectoryURL:(NSURL *)directory atIndex:(NSUInteger)index {	
	// grab a reference to the mutable proxy of our files child nodes
	NSMutableArray *childNodes = [file mutableChildNodes];
	NSFileManager *fm = [NSFileManager defaultManager];
	// array to hold files that represent directories so we can search for them later
	NSMutableArray *dFiles = [NSMutableArray array];
	BOOL copyFiles = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesAddFilesToProjectCopyFilesKey];
	
	// iterate over the passed in urls
	for (NSURL *url in urls) {
		NSURL *newURL = url;
		
		if ([[newURL lastPathComponent] hasPrefix:@"."])
			continue;
		
		if (copyFiles) {
			
			if ([fm copyItemAtURL:url toURL:[([file directoryURL] == nil)?directory:[file directoryURL] URLByAppendingPathComponent:[url lastPathComponent]] error:NULL])
				newURL = [([file directoryURL] == nil)?directory:[file directoryURL] URLByAppendingPathComponent:[url lastPathComponent]];
		}
		
		// create a new file for each url, which might have been modified if we had to copy the file
		WCFile *file = [WCFile fileWithURL:newURL];
		
		// if the given url represents a directory, we have extra work to do
		if ([fm directoryExistsAtURL:newURL]) {
			// add the file to our list for later searching
			[dFiles addObject:file];
			
			// create a directory enumerator for the url, this will loop through all paths contained at that url
			NSDirectoryEnumerator *de = [fm enumeratorAtURL:url includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles | NSDirectoryEnumerationSkipsPackageDescendants) errorHandler:nil];
			
			// loop through each sub url at our give url
			for (NSURL *surl in de) {
				if ([[surl lastPathComponent] hasPrefix:@"."])
					continue;
				
				// create another file for each sub url, which will be a child of "file" up above
				WCFile *sFile = [WCFile fileWithURL:surl];
				
				// add it to the list for later searching
				if ([fm directoryExistsAtURL:surl])
					[dFiles addObject:sFile];
				
				// we need to locate the proper parent for each sub file
				WCFile *parent = nil;
				NSString *dPath = [[surl path] stringByDeletingLastPathComponent];
				
				// just search through the list we have maintained, break when we find the matching path
				for (WCFile *f in dFiles) {
					if ([dPath isEqualToString:[[f URL] path]]) {
						parent = f;
						break;
					}
				}
				
				// add the sub file to its proper parent
				[[parent mutableChildNodes] addObject:sFile];
			}
		}
		
		// add the file to its parent's child nodes, increment the index
		[childNodes insertObject:file atIndex:index++];
	}
	return YES;
}

- (BOOL)addFilePaths:(NSArray *)paths toFile:(WCFile *)file atIndex:(NSUInteger)index; {
	NSMutableArray *urls = [NSMutableArray arrayWithCapacity:[paths count]];
	
	for (NSString *path in paths)
		[urls addObject:[NSURL fileURLWithPath:path]];
	
	return [self addFileURLs:urls toFile:file atIndex:index];
}

- (WCProject *)createProjectFromFolder:(NSURL *)folderURL error:(NSError **)error; {	
	// use the name of the folder as the name of our project
	NSString *projectName = [folderURL lastPathComponent];
	// the wrapper is the "file" the user sees, take the folder url, add the project name and tack on the extension
	NSURL *wrapperURL = [folderURL URLByAppendingPathComponent:[projectName stringByAppendingPathExtension:kWCProjectWrapperExtension]];
	// create the root dictionary for our project data file
	NSMutableDictionary *projectDict = [NSMutableDictionary dictionary];
	
	// add the version information
	[projectDict setObject:[NSNumber numberWithFloat:kWCProjectVersion] forKey:kWCProjectVersionKey];
	
	// create our root project file
	WCProjectFile *projectFile = [WCProjectFile fileWithURL:wrapperURL];
	
	// add all the files in the project folder to our root project file
	[self addFileURLsInDirectory:folderURL toFile:projectFile inDirectoryURL:folderURL];
	
	// add the dictionary representation to our project dict
	[projectDict setObject:projectFile forKey:kWCProjectFileKey];
	
	WCBuildTarget *target = [WCBuildTarget buildTargetWithOutputType:WCBuildTargetOutputTypeBinary outputName:projectName];
	// add a default build target to the project dict
	NSArray *targets = [NSArray arrayWithObjects:target, nil];
	[projectDict setObject:targets forKey:kWCProjectBuildTargetsKey];
	
	// create the data for the project data file
	NSData *pdata = [NSKeyedArchiver archivedDataWithRootObject:projectDict];
	// create the file wrapper for our project data file
	NSFileWrapper *dataWrapper = [[[NSFileWrapper alloc] initRegularFileWithContents:pdata] autorelease];
	
	// set the preferred name for the project data file wrapper
	[dataWrapper setPreferredFilename:kWCProjectDataFileName];
	
	// create the file wrapper for the project wrapper, which is really a folder
	NSFileWrapper *projectWrapper = [[[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil] autorelease];
	
	// add the data wrapper to the project wrapper
	[projectWrapper addFileWrapper:dataWrapper];
	
	// write the project wrapper to disk, bail if we fail
	if (![projectWrapper writeToURL:wrapperURL options:NSFileWrapperWritingAtomic originalContentsURL:nil error:error])
		return nil;
	
	return [[NSDocumentController sharedDocumentController] openDocumentWithContentsOfURL:wrapperURL display:YES error:error];
}

- (WCProject *)createProjectAtURL:(NSURL *)projectURL withTemplate:(WCProjectTemplate *)projectTemplate error:(NSError **)error; {
	NSURL *directoryURL = [projectURL URLByDeletingLastPathComponent];
	NSString *projectDirectoryName = [[projectURL lastPathComponent] stringByDeletingPathExtension];
	NSURL *projectDirectoryURL = [directoryURL URLByAppendingPathComponent:projectDirectoryName];
	
	// create the folder for our new project
	if (![[NSFileManager defaultManager] createDirectoryAtPath:[projectDirectoryURL path] withIntermediateDirectories:NO attributes:nil error:NULL])
		return nil;
	
	// copy any source code files in the template folder to the project folder
	if (![[NSFileManager defaultManager] copyItemsInDirectory:[projectTemplate templateURL] ofTypes:[NSArray arrayWithObjects:kWCFileAssemblyUTI,kWCFileIncludeUTI,kWCFilePanicCodaImportedUTI, nil] toURL:projectDirectoryURL error:NULL])
		return nil;
	
	[projectTemplate setProjectName:projectDirectoryName];
	
	NSURL *mainFile = [projectDirectoryURL URLByAppendingPathComponent:@"main.asm"];
	
	// try to copy our main.asm file, it should only fail if the user has included their own main.asm in the template folder
	[[NSFileManager defaultManager] copyItemAtURL:[[NSBundle mainBundle] URLForResource:@"main" withExtension:@"z80"] toURL:mainFile error:NULL];
	
	// handle the main.asm file, we have to process it and remove any placeholders present
	// make sure it exists first, either we copied it earlier from above or it was copied over from the template folder
	if ([[NSFileManager defaultManager] fileExistsAtURL:mainFile]) {
		// get the existing files' contents
		NSStringEncoding encoding = NSUTF8StringEncoding;
		NSString *oldString = [NSString stringWithContentsOfURL:mainFile usedEncoding:&encoding error:NULL];
		// replace the placeholders
		NSString *newString = [oldString stringByReplacingPlaceholdersWithTemplate:projectTemplate];
		
		// write the new file
		[newString writeToURL:mainFile atomically:YES encoding:encoding error:NULL];
		
		// rename the file appropriately
		[[NSFileManager defaultManager] moveItemAtURL:mainFile toURL:[[mainFile URLByDeletingLastPathComponent] URLByAppendingPathComponent:[projectDirectoryName stringByAppendingPathExtension:@"asm"]] error:NULL];
	}
	
	// copy any include files required by the template to the project folder
	NSURL *systemIncludeFilesDirectory = [[NSBundle mainBundle] URLForResource:@"Include Files" withExtension:@""];
	NSArray *systemIncludeFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtURL:systemIncludeFilesDirectory includingPropertiesForKeys:nil options:0 error:NULL];
	NSURL *userIncludeFilesDirectory = [[WCGeneralPerformer sharedPerformer] userIncludeFilesURL];
	NSArray *userIncludeFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtURL:userIncludeFilesDirectory includingPropertiesForKeys:nil options:0 error:NULL];
	BOOL preferUserIncludeFiles = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesAdvancedProjectTemplatesPreferUserIncludeFilesKey];
	
	for (NSString *includeFile in [projectTemplate includeFileNames]) {
		NSString *includeFileWithoutExtension = [includeFile stringByDeletingPathExtension];
		
		// search for a matching system include file first
		NSURL *mSystemIncludeFile = nil;
		for (NSURL *url in systemIncludeFiles) {
			if ([[includeFileWithoutExtension lowercaseString] isEqualToString:[[[url lastPathComponent] stringByDeletingPathExtension] lowercaseString]]) {
				mSystemIncludeFile = url;
				break;
			}
		}
		
		// if we aren't supposed to prefer user include files, try the copy and continue to the next include file if it fails
		if (!preferUserIncludeFiles && mSystemIncludeFile) {
			if (![[NSFileManager defaultManager] copyItemAtURL:mSystemIncludeFile toURL:[projectDirectoryURL URLByAppendingPathComponent:[[includeFile lowercaseString] stringByAppendingPathExtension:@"inc"]] error:NULL])
				continue;
		}
		
		// search for a matching user include file
		NSURL *mUserIncludeFile = nil;
		for (NSURL *url in userIncludeFiles) {
			if ([[includeFileWithoutExtension lowercaseString] isEqualToString:[[[url lastPathComponent] stringByDeletingPathExtension] lowercaseString]]) {
				mUserIncludeFile = url;
				break;
			}
		}
		
		// if copying the user include file succeeds, continue to the next include file
		if (mUserIncludeFile)
			if ([[NSFileManager defaultManager] copyItemAtURL:mUserIncludeFile toURL:[projectDirectoryURL URLByAppendingPathComponent:[[includeFile lowercaseString] stringByAppendingPathExtension:@"inc"]] error:NULL])
				continue;
		
		// try copying the matching system include file if copying the user include file fails
		if (mSystemIncludeFile)
			[[NSFileManager defaultManager] copyItemAtURL:mSystemIncludeFile toURL:[projectDirectoryURL URLByAppendingPathComponent:[[includeFile lowercaseString] stringByAppendingPathExtension:@"inc"]] error:NULL];
	}
	return [self createProjectFromFolder:projectDirectoryURL error:error];
}

- (NSFileWrapper *)fileWrapperForProject:(WCProject *)project error:(NSError **)outError; {
	NSMutableDictionary *projectDict = [NSMutableDictionary dictionary];
	[projectDict setObject:[NSNumber numberWithFloat:kWCProjectVersion] forKey:kWCProjectVersionKey];
	
	WCProjectFile *projectFile = [project projectFile];
	[projectDict setObject:projectFile forKey:kWCProjectFileKey];
	
	NSArray *buildTargets = [project buildTargets];
	[projectDict setObject:buildTargets forKey:kWCProjectBuildTargetsKey];
	
	NSData *pdata = [NSKeyedArchiver archivedDataWithRootObject:projectDict];
	NSFileWrapper *projectWrapper = [[[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil] autorelease];
	NSFileWrapper *dataWrapper = [[[NSFileWrapper alloc] initRegularFileWithContents:pdata] autorelease];
	
	[dataWrapper setPreferredFilename:kWCProjectDataFileName];
	
	NSData *sdata = [NSPropertyListSerialization dataWithPropertyList:[project projectSettings] format:NSPropertyListXMLFormat_v1_0 options:NSPropertyListImmutable error:NULL];
	NSFileWrapper *settingsWrapper = [[[NSFileWrapper alloc] initRegularFileWithContents:sdata] autorelease];
	
	[settingsWrapper setPreferredFilename:[NSUserName() stringByAppendingPathExtension:@"wcodesettings"]];
	
	[projectWrapper addFileWrapper:dataWrapper];
	[projectWrapper addFileWrapper:settingsWrapper];
	
	return projectWrapper;
	
}

- (WCFile *)createFileAtURL:(NSURL *)fileURL withTemplate:(WCFileTemplate *)fileTemplate error:(NSError **)error; {
	// get the full path to the template file
	[fileTemplate setFileName:[fileURL lastPathComponent]];
	NSStringEncoding encoding = NSUTF8StringEncoding;
	NSURL *templateFileURL = [[fileTemplate templateURL] URLByAppendingPathComponent:[fileTemplate templateFileName]];
	NSString *oldString = [NSString stringWithContentsOfURL:templateFileURL usedEncoding:&encoding error:error];
	
	if (!oldString)
		return nil;
	
	NSString *newString = [oldString stringByReplacingPlaceholdersWithFileTemplate:fileTemplate];
	
	if (![newString writeToURL:fileURL atomically:YES encoding:encoding error:error])
		return nil;
	
	return [WCFile fileWithURL:fileURL];
}

- (NSURL *)userProjectTemplatesURL; {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	
	if (![paths count])
		return nil;

	NSString *path = [[NSFileManager defaultManager] createDirectoryAtPath:[[paths objectAtIndex:0] stringByAppendingPathComponent:[[[NSProcessInfo processInfo] processName] stringByAppendingPathComponent:NSLocalizedString(@"Project Templates", @"Project Templates")]] error:NULL];
	
	if (!path)
		return nil;
	return [NSURL fileURLWithPath:path isDirectory:YES];
}
- (NSURL *)userFileTemplatesURL; {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	
	if (![paths count])
		return nil;
	
	NSString *path = [[NSFileManager defaultManager] createDirectoryAtPath:[[paths objectAtIndex:0] stringByAppendingPathComponent:[[[NSProcessInfo processInfo] processName] stringByAppendingPathComponent:NSLocalizedString(@"File Templates", @"File Templates")]] error:NULL];
	
	if (!path)
		return nil;
	return [NSURL fileURLWithPath:path isDirectory:YES];
}
- (NSURL *)userIncludeFilesURL; {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	
	if (![paths count])
		return nil;
	
	NSString *path = [[NSFileManager defaultManager] createDirectoryAtPath:[[paths objectAtIndex:0] stringByAppendingPathComponent:[[[NSProcessInfo processInfo] processName] stringByAppendingPathComponent:NSLocalizedString(@"Include Files", @"Include Files")]] error:NULL];
	
	if (!path)
		return nil;
	return [NSURL fileURLWithPath:path isDirectory:YES];
}
@end
