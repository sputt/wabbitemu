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
#import "WCProjectBuildTargetPopUpButton.h"
#import "NSColor-NTExtensions.h"
#import "WCDocument.h"
#import "WCDocumentController.h"
#import "WCProjectFilesOutlineViewController.h"
#import "NSTreeController+WCExtensions.h"
#import "NSWindow-NoodleEffects.h"
#import "WCBreakpoint.h"
#import "NSGradient+WCExtensions.h"


NSString *const kWCProjectToolbarBuildTargetPopUpButtonItemIdentifier = @"kWCProjectToolbarBuildTargetPopUpButtonItemIdentifier";
NSString *const kWCProjectToolbarBuildItemIdentifier = @"kWCProjectToolbarBuildItemIdentifier";
NSString *const kWCProjectToolbarBuildAndRunItemIdentifier = @"kWCProjectToolbarBuildAndRunItemIdentifier";
NSString *const kWCProjectToolbarBuildAndDebugItemIdentifer = @"kWCProjectToolbarBuildAndDebugItemIdentifer";

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
	// get the updated set of all file paths in the project
	NSSet *currentFilePaths = [[file project] absoluteFilePaths];
	// grab a reference to the mutable proxy of our files child nodes
	NSMutableArray *childNodes = [file mutableChildNodes];
	NSFileManager *fm = [NSFileManager defaultManager];
	// array to hold files that represent directories so we can search for them later
	NSMutableArray *dFiles = [NSMutableArray array];
	BOOL copyFiles = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesAddFilesToProjectCopyFilesKey];
	
	// iterate over the passed in urls
	for (NSURL *url in urls) {
		NSURL *newURL = url;
		
		if ([currentFilePaths containsObject:[newURL path]])
			continue;
		
		if ([[newURL lastPathComponent] hasPrefix:@"."])
			continue;
		
		if (copyFiles) {
			
			if ([fm copyItemAtURL:url toURL:[([file directoryURL] == nil)?directory:[file directoryURL] URLByAppendingPathComponent:[url lastPathComponent]] error:NULL])
				newURL = [([file directoryURL] == nil)?directory:[file directoryURL] URLByAppendingPathComponent:[url lastPathComponent]];
		}
		
		// create a new file for each url, which might have been modified if we had to copy the file
		WCFile *nFile = [WCFile fileWithURL:newURL];
		
		// if the given url represents a directory, we have extra work to do
		if ([fm directoryExistsAtURL:newURL]) {
			// add the file to our list for later searching
			[dFiles addObject:nFile];
			
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
		[childNodes insertObject:nFile atIndex:index++];
	}
	return YES;
}

- (BOOL)addFilePaths:(NSArray *)paths toFile:(WCFile *)file atIndex:(NSUInteger)index; {
	NSMutableArray *urls = [NSMutableArray arrayWithCapacity:[paths count]];
	
	for (NSString *path in paths)
		[urls addObject:[NSURL fileURLWithPath:path]];
	
	return [self addFileURLs:urls toFile:file atIndex:index];
}

- (BOOL)addDocument:(WCDocument *)document toProject:(WCProject *)project; {
	// check to see if the file already exists in the project
	if ([[project absoluteFilePaths] containsObject:[[document fileURL] path]])
		return NO;
	
	// saving before hand just makes it easier, we can close the document instance without any problem
	[document saveDocument:nil];
	
	// remove the text view's layout manager from from the file's text storage, or else it won't be dealloc'd
	[[[document file] textStorage] removeLayoutManager:[[[document fileViewController] textView] layoutManager]];
	// since we saved the file, remove all undo actions, this allows the text view to be dealloc'd
	[[[document file] undoManager] removeAllActions];
	// set the ruler view's client to nil which causes the ruler view to unregister itself to certain notifications, otherwise we get a crash, which is bad
	[[[[[document fileViewController] textView] enclosingScrollView] verticalRulerView] setClientView:nil];
	// insert the document's file into the project's root
	[[[project projectFile] mutableChildNodes] insertObject:[document file] atIndex:0];
	// select the new file in the files outline view
	[(NSTreeController *)[[[project projectFilesOutlineViewController] outlineView] dataSource] setSelectedRepresentedObject:[document file]];
	
	// switch to the files outline view so the user can see the new file
	[project viewProject:nil];
	
	// grab the rect of the new file in the files outline view, converting it to the window's coordinate system
	NSRect rect = [[[project projectFilesOutlineViewController] outlineView] convertRectToBase:[[[project projectFilesOutlineViewController] outlineView] frameOfCellAtColumn:0 row:[[[project projectFilesOutlineViewController] outlineView] rowForItem:[(NSTreeController *)[[[project projectFilesOutlineViewController] outlineView] dataSource] treeNodeForRepresentedObject:[document file]]]]];
	// convert the origin to the screen's coordinate system
	rect.origin = [[project windowForSheet] convertBaseToScreen:rect.origin];
	
#ifdef DEBUG
	NSAssert(!NSIsEmptyRect(rect), @"cannot zoom off the screen with an empty rect!");
#endif
	
	// animate the document window into the rect of the new file
	[[document windowForSheet] zoomOffToRect:rect];
	
	// close the actual document
	[document close];
	
	// notify the project that a new file has been added; forces text views to re-highlight, symbols to update, etc
	[project noteNumberOfFilesChanged];
	
	return YES;
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

- (NSColor *)findBackgroundColor; {
	return [[NSColor yellowColor] colorWithAlphaComponent:0.5];
}
- (NSColor *)findUnderlineColor; {
	return [[NSColor orangeColor] colorWithAlphaComponent:0.75];
}

- (NSDictionary *)findAttributes; {
	return [NSDictionary dictionaryWithObjectsAndKeys:[self findBackgroundColor],NSBackgroundColorAttributeName,[NSNumber numberWithUnsignedInteger:NSUnderlinePatternSolid|NSUnderlineStyleSingle],NSUnderlineStyleAttributeName,[self findUnderlineColor],NSUnderlineColorAttributeName, nil];
}
#define CORNER_RADIUS 3.0
#define RULER_MARGIN 3.0
- (void)drawBreakpoint:(WCBreakpoint *)breakpoint inRect:(NSRect)rect {
	static NSGradient *activeGradient = nil;
	static NSGradient *inactiveGradient = nil;
	static NSColor *activeStroke = nil;
	static NSColor *inactiveStroke = nil;
	
	if (!activeGradient) {
		// xcode like colors
		/*
		activeGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.431 green:0.608 blue:0.792 alpha:1.0] endingColor:[NSColor colorWithCalibratedRed:0.329 green:0.533 blue:0.757 alpha:1.0]];
		inactiveGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.431 green:0.608 blue:0.792 alpha:0.5] endingColor:[NSColor colorWithCalibratedRed:0.329 green:0.533 blue:0.757 alpha:0.5]];
		activeStroke = [[NSColor colorWithCalibratedRed:0.235 green:0.443 blue:0.686 alpha:1.0] retain];
		inactiveStroke = [[NSColor colorWithCalibratedRed:0.235 green:0.443 blue:0.686 alpha:0.5] retain];
		*/
		
		// red colors
		/*
		activeGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.85 green:0.0 blue:0.0 alpha:1.0] endingColor:[NSColor colorWithCalibratedRed:0.65 green:0.0 blue:0.0 alpha:1.0]];
		activeStroke = [[NSColor colorWithCalibratedRed:0.5 green:0.0 blue:0.0 alpha:1.0] retain];
		inactiveGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.85 green:0.0 blue:0.0 alpha:0.5] endingColor:[NSColor colorWithCalibratedRed:0.65 green:0.0 blue:0.0 alpha:0.5]];
		inactiveStroke = [[NSColor colorWithCalibratedRed:0.5 green:0.0 blue:0.0 alpha:0.5] retain];
		 */
		// purple colors
		/*
		activeGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.65 green:0.0 blue:0.65 alpha:1.0] endingColor:[NSColor colorWithCalibratedRed:0.45 green:0.0 blue:0.45 alpha:1.0]];
		activeStroke = [[NSColor colorWithCalibratedRed:0.35 green:0.0 blue:0.35 alpha:1.0] retain];
		inactiveGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.65 green:0.0 blue:0.65 alpha:0.5] endingColor:[NSColor colorWithCalibratedRed:0.45 green:0.0 blue:0.45 alpha:0.5]];
		inactiveStroke = [[NSColor colorWithCalibratedRed:0.35 green:0.0 blue:0.35 alpha:0.5] retain];
		 */
		activeGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.6 green:0.6 blue:0.6 alpha:1.0] endingColor:[NSColor colorWithCalibratedRed:0.35 green:0.35 blue:0.35 alpha:1.0]];
		activeStroke = [[NSColor colorWithCalibratedRed:0.25 green:0.25 blue:0.25 alpha:1.0] retain];
		inactiveGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.6 green:0.6 blue:0.6 alpha:0.5] endingColor:[NSColor colorWithCalibratedRed:0.35 green:0.35 blue:0.35 alpha:0.5]];
		inactiveStroke = [[NSColor colorWithCalibratedRed:0.25 green:0.25 blue:0.25 alpha:0.5] retain];
	}
	
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path setLineJoinStyle:NSRoundLineJoinStyle];
	
	NSRect bRect = NSInsetRect(rect, 0.0, 1.0);
	bRect = NSMakeRect(NSMinX(bRect)+2.0, NSMinY(bRect), NSWidth(rect) - 2.0, NSHeight(bRect));
	
	[path moveToPoint:NSMakePoint(NSMaxX(bRect), NSMinY(bRect) + floor(NSHeight(rect) / 2))];
	[path lineToPoint:NSMakePoint(NSMaxX(bRect) - 5.0, NSMaxY(bRect))];
	
	[path appendBezierPathWithArcWithCenter:NSMakePoint(NSMinX(bRect) + CORNER_RADIUS, NSMaxY(bRect) - CORNER_RADIUS) radius:CORNER_RADIUS startAngle:90 endAngle:180];
	
	[path appendBezierPathWithArcWithCenter:NSMakePoint(NSMinX(bRect) + CORNER_RADIUS, NSMinY(bRect) + CORNER_RADIUS) radius:CORNER_RADIUS startAngle:180 endAngle:270];
	[path lineToPoint:NSMakePoint(NSMaxX(bRect) - 5.0, NSMinY(bRect))];
	[path closePath];
	
	if ([breakpoint isActive])
		[activeGradient drawInBezierPath:path angle:90.0];
	else
		[inactiveGradient drawInBezierPath:path angle:90.0];
	
	if ([breakpoint isActive])
		[activeStroke setStroke];
	else
		[inactiveStroke setStroke];
	[path setLineWidth:1.5];
	[path stroke];
}
@end
