//
//  WCFile.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFile.h"
#import "NSFileManager+WCExtensions.h"
#import "WCTextStorage.h"
#import "NSImage+WCExtensions.h"
#import "WCSymbolScanner.h"
#import "WCProject.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCBuildMessage.h"
#import "WCAlias.h"
#import "NSString+WCExtensions.h"
#import "WCBreakpoint.h"


NSString *const kWCFileAssemblyUTI = @"org.revsoft.wabbitcode.assembly";
NSString *const kWCFileIncludeUTI = @"org.revsoft.wabbitcode.include";
NSString *const kWCFilePanicCodaImportedUTI = @"com.panic.coda.active-server-include-file";

NSString *const kWCFileHasUnsavedChangesNotification = @"kWCFileHasUnsavedChangesNotification";

NSString *const kWCFileNumberOfErrorMessagesChangedNotification = @"kWCFileNumberOfErrorMessagesChangedNotification";
NSString *const kWCFileNumberOfWarningMessagesChangedNotification = @"kWCFileNumberOfWarningMessagesChangedNotification";

NSString *const kWCFileNumberOfBreakpointsDidChangeNotification = @"kWCFileNumberOfBreakpointsDidChangeNotification";

NSString *const kWCFileNameDidChangeNotification = @"kWCFileNameDidChangeNotification";

static NSMutableDictionary *_UTIsToUnsavedIcons = nil;

@interface WCFile (Private)
- (void)_setupTextStorageAndSymbolScanner;
@end

@implementation WCFile
#pragma mark *** Subclass Overrides ***
+ (void)initialize {
	if ([WCFile class] != self)
		return;
	
	_UTIsToUnsavedIcons = [[NSMutableDictionary alloc] init];
}

- (NSString *)description {
	return [NSString stringWithFormat:@"class: %@ name: %@",[self className],[self name]];
}

- (void)dealloc {
	/*
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	 */
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_lineNumbersToWarningMessages release];
	[_lineNumbersToErrorMessages release];
	[_symbolScanner release];
	[_textStorage release];
	[_undoManager release];
	[_textViewForFindInProjectReplace release];
	[_UUID release];
	[_alias release];
    [super dealloc];
}

- (NSDictionary *)dictionaryRepresentation {
	// grab super's dictionary
	NSMutableDictionary *retval = [NSMutableDictionary dictionaryWithDictionary:[super dictionaryRepresentation]];
	
	// add our keys to it
	[retval addEntriesFromDictionary:[NSDictionary dictionaryWithObjectsAndKeys:[[self alias] dictionaryRepresentation],@"alias", nil]];
	
	// return a copy
	return [[retval copy] autorelease];
}

- (BOOL)isLeaf {
	if ([self isDirectory])
		return NO;
	return YES;
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSKeyValueObserving
+ (NSSet *)keyPathsForValuesAffectingValueForKey:(NSString *)key {
	if ([key isEqualToString:@"hasUnsavedChanges"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObject:@"changeCount"];
	else if ([key isEqualToString:@"icon"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObject:@"hasUnsavedChanges"];
	else if ([key isEqualToString:@"isEdited"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObject:@"hasUnsavedChanges"];
	else if ([key isEqualToString:@"name"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObjectsFromArray:[NSArray arrayWithObjects:@"URL",@"bookmarkData", nil]];
	return [super keyPathsForValuesAffectingValueForKey:key];
}
#pragma mark NSCoding
- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:[self alias] forKey:@"alias"];
	[coder encodeObject:[self UUID] forKey:@"UUID"];
	[coder encodeObject:[self allBreakpoints] forKey:@"breakpoints"];
	[super encodeWithCoder:coder];
}
- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_alias = [[coder decodeObjectForKey:@"alias"] retain];
	_UUID = [[coder decodeObjectForKey:@"UUID"] retain];
	
	NSArray *breakpoints = [coder decodeObjectForKey:@"breakpoints"];
	if ([breakpoints count]) {
		_lineNumbersToBreakpoints = [[NSMutableDictionary alloc] initWithCapacity:[breakpoints count]];
		
		for (WCBreakpoint *bp in breakpoints) {
			[bp setFile:self];
			[_lineNumbersToBreakpoints setObject:bp forKey:[NSNumber numberWithUnsignedInteger:[bp lineNumber]]];
		}
	}
	
	return self;
}
#pragma mark NSCopying
- (id)copyWithZone:(NSZone *)zone {
	WCFile *copy = [super copyWithZone:zone];
	
	copy->_alias = [_alias retain];
	copy->_UUID = [_UUID retain];
	
	return copy;
}
#pragma mark *** Public Methods ***
- (BOOL)saveFile:(NSError **)outError; {
	NSString *string = [[self textStorage] string];
	
	if (![string writeToURL:[self URL] atomically:YES encoding:_encoding error:outError])
		return NO;
	
	[self setChangeCount:0];
	
	return YES;
}
- (BOOL)resetFile:(NSError **)outError; {
	NSString *string = [[[NSString alloc] initWithContentsOfURL:[self URL] usedEncoding:&_encoding error:NULL] autorelease];
	
	[[self textStorage] replaceCharactersInRange:NSMakeRange(0, [[[self textStorage] string] length]) withString:string];
	
	[self setChangeCount:0];
	
	return YES;
}
#pragma mark Creation
+ (id)fileWithURL:(NSURL *)url; {
	return [[[[self class] alloc] initWithURL:url] autorelease];
}
- (id)initWithURL:(NSURL *)url; {
	return [self initWithURL:url name:nil];
}
+ (id)fileWithURL:(NSURL *)url name:(NSString *)name; {
	return [[[[self class] alloc] initWithURL:url name:name] autorelease];
}
- (id)initWithURL:(NSURL *)url name:(NSString *)name; {
	if (!(self = [super initWithName:name]))
		return nil;
	
	_alias = [[WCAlias alloc] initWithURL:url];
	_UUID = [[NSString UUIDString] retain];
	
	return self;
}
#pragma mark Errors & Warnings
- (void)addErrorMessage:(WCBuildMessage *)error; {
	if (!_lineNumbersToErrorMessages)
		_lineNumbersToErrorMessages = [[NSMutableDictionary alloc] init];
	
	[_lineNumbersToErrorMessages setObject:error forKey:[NSNumber numberWithUnsignedInteger:[error lineNumber] - 1]];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNumberOfErrorMessagesChangedNotification object:self];
}
- (void)addWarningMessage:(WCBuildMessage *)warning; {
	if (!_lineNumbersToWarningMessages)
		_lineNumbersToWarningMessages = [[NSMutableDictionary alloc] init];
	
	[_lineNumbersToWarningMessages setObject:warning forKey:[NSNumber numberWithUnsignedInteger:[warning lineNumber] - 1]];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNumberOfWarningMessagesChangedNotification object:self];
}
- (WCBuildMessage *)errorMessageAtLineNumber:(NSUInteger)lineNumber; {
	return [_lineNumbersToErrorMessages objectForKey:[NSNumber numberWithUnsignedInteger:lineNumber]];
}
- (WCBuildMessage *)warningMessageAtLineNumber:(NSUInteger)lineNumber; {
	return [_lineNumbersToWarningMessages objectForKey:[NSNumber numberWithUnsignedInteger:lineNumber]];
}
- (void)removeAllErrorMessages; {
	[_lineNumbersToErrorMessages removeAllObjects];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNumberOfErrorMessagesChangedNotification object:self];
}
- (void)removeAllWarningMessages; {
	[_lineNumbersToWarningMessages removeAllObjects];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNumberOfWarningMessagesChangedNotification object:self];
}
- (void)removeAllBuildMessages; {
	[self removeAllErrorMessages];
	[self removeAllWarningMessages];
}
- (NSArray *)allErrorMessages; {
	return [_lineNumbersToErrorMessages allValues];
}
- (NSArray *)allWarningMessages; {
	return [_lineNumbersToWarningMessages allValues];
}
- (NSUInteger)lineStartForBuildMessage:(WCBuildMessage *)message; {
	return [[self textStorage] lineStartIndexForLineNumber:[message lineNumber]-1];
}
#pragma mark Breakpoints
- (void)addBreakpoint:(WCBreakpoint *)breakpoint; {
	if (!_lineNumbersToBreakpoints)
		_lineNumbersToBreakpoints = [[NSMutableDictionary alloc] init];
	
	[_lineNumbersToBreakpoints setObject:breakpoint forKey:[NSNumber numberWithUnsignedInteger:[breakpoint lineNumber]]];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNumberOfBreakpointsDidChangeNotification object:self];
}
- (void)removeBreakpoint:(WCBreakpoint *)breakpoint; {
	[_lineNumbersToBreakpoints removeObjectForKey:[NSNumber numberWithUnsignedInteger:[breakpoint lineNumber]]];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNumberOfBreakpointsDidChangeNotification object:self];
}
- (WCBreakpoint *)breakpointAtLineNumber:(NSUInteger)lineNumber; {
	return [_lineNumbersToBreakpoints objectForKey:[NSNumber numberWithUnsignedInteger:lineNumber]];
}
- (NSArray *)allBreakpoints; {
	return [_lineNumbersToBreakpoints allValues];
}
- (NSArray *)allBreakpointsSortedByLineNumber; {
	static NSArray *sortDescriptors = nil;
	if (!sortDescriptors)
		sortDescriptors = [[NSArray alloc] initWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"lineNumber" ascending:YES selector:@selector(compare:)] autorelease],nil];
	
	NSMutableArray *retval = [NSMutableArray arrayWithArray:[self allBreakpoints]];
	
	[retval sortUsingDescriptors:sortDescriptors];
	
	return [[retval copy] autorelease];
}
#pragma mark Accessors
- (NSString *)name {
	if ([super name] && [self isDirectory])
		return [super name];
	return [[[self alias] absolutePathForDisplay] lastPathComponent];
}
- (void)setName:(NSString *)name {
	// if it's a group, just rename normally
	if ([self isDirectory]) {
		[super setName:name];
		return;
	}
	
	// otherwise we need to actually rename the represented file
	// check to make sure the name is actually different
	if ([name isEqualToString:[self name]])
		return;
	
	// get our unique file path for renaming
	NSString *renamePath = [[NSFileManager defaultManager] uniqueFilePathForPath:[[[self directoryURL] path] stringByAppendingPathComponent:name]];
	
	// rename by moving
	if (![[NSFileManager defaultManager] moveItemAtPath:[self absolutePath] toPath:renamePath error:NULL])
		return;
	
	// post our notification, this is mainly for the tabs if the given file is open
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileNameDidChangeNotification object:self];
}

@synthesize alias=_alias;
@dynamic URL;
- (NSURL *)URL {
	return [[self alias] URL];
}
- (void)setURL:(NSURL *)URL {
	if (!_alias)
		_alias = [[WCAlias alloc] initWithURL:URL];
	else
		[[self alias] setURL:URL];
	
#ifdef DEBUG
    NSAssert(_alias != nil, @"alias cannot be nil!");
#endif
}

- (NSImage *)icon {
	if ([[NSFileManager defaultManager] directoryExistsAtURL:[self URL]] &&
		![[NSWorkspace sharedWorkspace] isFilePackageAtPath:[[self URL] path]])
		return [NSImage imageNamed:@"Group16x16"];
	
	NSImage *icon = [[NSWorkspace sharedWorkspace] iconForFile:[self.URL path]];
	
	if ([self hasUnsavedChanges]) {
		NSImage *unsavedIcon = [_UTIsToUnsavedIcons objectForKey:[self UTI]];
		
		if (!unsavedIcon) {
			unsavedIcon = [icon unsavedIconFromImage];
			
			[_UTIsToUnsavedIcons setObject:unsavedIcon forKey:[self UTI]];
		}
		
		icon = unsavedIcon;
	}
	
	[icon setSize:NSMakeSize(16.0, 16.0)];
	
	return icon;
}

@dynamic symbolScanner;
- (WCSymbolScanner *)symbolScanner {
	[self _setupTextStorageAndSymbolScanner];
	
	return _symbolScanner;
}

@dynamic isDirectory;
- (BOOL)isDirectory {
	return [[self alias] isDirectory];
}

@dynamic absolutePath;
- (NSString *)absolutePath {
	return [[self URL] path];
}

@dynamic absolutePathForDisplay;
- (NSString *)absolutePathForDisplay {
	return [[self absolutePath] stringByReplacingPercentEscapesUsingEncoding:[self encoding]];
}

@dynamic directoryURL;
- (NSURL *)directoryURL {
	return ([self isDirectory])?[self URL]:[[self URL] URLByDeletingLastPathComponent];
}

@dynamic textStorage;
- (WCTextStorage *)textStorage {
	[self _setupTextStorageAndSymbolScanner];
	
	return _textStorage;
}

@dynamic undoManager;
- (NSUndoManager *)undoManager {
	if (!_undoManager) {
		_undoManager = [[NSUndoManager alloc] init];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_didOpenUndoGroup:) name:NSUndoManagerDidOpenUndoGroupNotification object:_undoManager];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_didUndoChange:) name:NSUndoManagerDidUndoChangeNotification object:_undoManager];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_didRedoChange:) name:NSUndoManagerDidRedoChangeNotification object:_undoManager];
	}
	return _undoManager;
}

@dynamic hasUnsavedChanges;
- (BOOL)hasUnsavedChanges {
	return ([self changeCount] == 0)?NO:YES;
}
- (BOOL)isEdited {
	return [self hasUnsavedChanges];
}
@dynamic UTI;
- (NSString *)UTI {
	return [[self alias] UTI];
}

@dynamic project;
- (WCProject *)project {
	if (!_project)
		_project = [[self parentNode] project];
	return _project;
}
- (void)setProject:(WCProject *)project {
	_project = project;
}
@dynamic canEditName;
- (BOOL)canEditName {
	return [[self URL] checkResourceIsReachableAndReturnError:NULL];
}
@synthesize UUID=_UUID;
@dynamic textViewForFindInProjectReplace;
- (NSTextView *)textViewForFindInProjectReplace {
	if (!_textViewForFindInProjectReplace) {
		_textViewForFindInProjectReplace = [[NSTextView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 1000.0, FLT_MAX)];
		
		[_textViewForFindInProjectReplace setUsesFindPanel:NO];
		[_textViewForFindInProjectReplace setAllowsUndo:YES];
		[_textViewForFindInProjectReplace setUsesFontPanel:NO];
		[_textViewForFindInProjectReplace setUsesRuler:NO];
		[_textViewForFindInProjectReplace setSmartInsertDeleteEnabled:NO];
		[_textViewForFindInProjectReplace setGrammarCheckingEnabled:NO];
		[_textViewForFindInProjectReplace setContinuousSpellCheckingEnabled:NO];
		[_textViewForFindInProjectReplace setDelegate:self];
		
		[[_textViewForFindInProjectReplace layoutManager] replaceTextStorage:[self textStorage]];
	}
	return _textViewForFindInProjectReplace;
}
- (NSUndoManager *)undoManagerForTextView:(NSTextView *)view {
	if (view == [self textViewForFindInProjectReplace])
		return [self undoManager];
	return nil;
}
@synthesize encoding=_encoding;
@dynamic changeCount;
- (NSInteger)changeCount; {
	return _changeCount;
}
- (void)setChangeCount:(NSInteger)value; {
	if (_changeCount == value)
		return;
	
	_changeCount = value;
	
	if (_changeCount == 0 || _changeCount == 1 || _changeCount == -1)
		[[NSNotificationCenter defaultCenter] postNotificationName:kWCFileHasUnsavedChangesNotification object:self];
}
#pragma mark *** Private Methods ***
- (void)_setupTextStorageAndSymbolScanner; {
	if (!_textStorage) {
		_encoding = NSUTF8StringEncoding;
		NSString *string = nil;
		if ([self URL])
			string = [[[NSString alloc] initWithContentsOfURL:[self URL] usedEncoding:&_encoding error:NULL] autorelease];
		else
			string = @"";
		
		_textStorage = [[WCTextStorage alloc] initWithString:string attributes:[NSDictionary dictionaryWithObjectsAndKeys:[[NSUserDefaults standardUserDefaults] fontForKey:kWCPreferencesEditorFontKey],NSFontAttributeName, nil]];
		
		_symbolScanner = [[WCSymbolScanner alloc] initWithFile:self];
	}
}
#pragma mark Notifications
- (void)_didOpenUndoGroup:(NSNotification *)note {
	[self setChangeCount:[self changeCount] + 1];
}
- (void)_didUndoChange:(NSNotification *)note {
	[self setChangeCount:[self changeCount] - 1];
}
- (void)_didRedoChange:(NSNotification *)note {
	[self setChangeCount:[self changeCount] + 1];
}
@end
