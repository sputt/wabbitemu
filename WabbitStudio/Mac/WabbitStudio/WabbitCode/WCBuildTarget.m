//
//  WCBuildTarget.m
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildTarget.h"
#import "WCProject.h"
#import "WCFile.h"
#import "NSImage+WCExtensions.h"
#import "WCBuildDefine.h"
#import "WCIncludeDirectory.h"
#import "WCAlias.h"

NSString *const kWCBuildTargetLabelsAreCaseSensitiveDidChangeNotification = @"kWCBuildTargetLabelsAreCaseSensitiveDidChangeNotification";
NSString *const kWCBuildTargetNameDidChangeNotification = @"kWCBuildTargetNameDidChangeNotification";
NSString *const kWCBuildTargetInputFileDidChangeNotification = @"kWCBuildTargetInputFileDidChangeNotification";
NSString *const kWCBuildTargetOutputNameDidChangeNotification = @"kWCBuildTargetOutputNameDidChangeNotification";
NSString *const kWCBuildTargetOutputTypeDidChangeNotification = @"kWCBuildTargetOutputTypeDidChangeNotification";

static NSString* const kWCBuildTargetOutputTypeKey = @"outputType";
static NSString* const kWCBuildTargetOutputNameKey = @"outputName";
static NSString* const kWCBuildTargetInputFileUUIDKey = @"inputFileUUID";
static NSString* const kWCBuildTargetIncludeDirectoriesKey = @"includeDirectories";
static NSString* const kWCBuildTargetDefinesKey = @"defines";
static NSString* const kWCBuildTargetGenerateCodeListingKey = @"generateCodeListing";
static NSString* const kWCBuildTargetGenerateLabelFileKey = @"generateLabelFile";
static NSString* const kWCBuildTargetLabelsAreCaseSensitiveKey = @"labelsAreCaseSensitive";

static NSDictionary *_outputTypesToFileExtensions = nil;

@implementation WCBuildTarget
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	_project = nil;
	[_inputFileAlias release];
	[_defines release];
	[_includeDirectories release];
	[_outputName release];
    [super dealloc];
}

+ (NSSet *)keyPathsForValuesAffectingValueForKey:(NSString *)key {
	if ([key isEqualToString:@"icon"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObjectsFromSet:[NSSet setWithObjects:@"isActive", nil]];
	else
		return [super keyPathsForValuesAffectingValueForKey:key];
}

- (void)setName:(NSString *)name {
	if ([[self name] isEqualToString:name])
		return;
	
	[super setName:name];
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCBuildTargetNameDidChangeNotification object:self];
}

- (NSImage *)icon {
	NSImage *retval = [NSImage imageNamed:@"Calculator16x16"];
	if ([self isActive])
		retval = [retval badgedImageWithImage:[NSImage imageNamed:@"GreenCheck8x8"] badgePosition:WCImageBadgePositionLowerRight];
	
	return retval;
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSCopying
- (id)copyWithZone:(NSZone *)zone {
	WCBuildTarget *copy = [super copyWithZone:zone];
	
	copy->_project = _project;
	copy->_outputType = _outputType;
	copy->_outputName = [_outputName retain];
	copy->_inputFileAlias = [_inputFileAlias retain];
	copy->_includeDirectories = [_includeDirectories retain];
	copy->_defines = [_defines retain];
	copy->_generateLabelFile = _generateLabelFile;
	copy->_generateCodeListing = _generateCodeListing;
	copy->_symbolsAreCaseSensitive = _symbolsAreCaseSensitive;
	copy->_isActive = _isActive;
	
	return copy;
}
#pragma mark NSMutableCopying
- (id)mutableCopyWithZone:(NSZone *)zone {
	WCBuildTarget *copy = [super mutableCopyWithZone:zone];
	
	copy->_project = _project;
	copy->_outputType = _outputType;
	copy->_outputName = [_outputName copy];
	copy->_inputFileAlias = [_inputFileAlias retain];
	copy->_includeDirectories = [[NSMutableArray alloc] initWithCapacity:[_includeDirectories count]];
	for (WCIncludeDirectory *idir in _includeDirectories)
		[copy addObjectToIncludeDirectories:[[idir mutableCopyWithZone:zone] autorelease]]; 
	copy->_defines = [[NSMutableArray alloc] initWithCapacity:[_defines count]];
	for (WCBuildDefine *def in _defines)
		[copy addObjectToDefines:[[def mutableCopyWithZone:zone] autorelease]];
	copy->_generateLabelFile = _generateLabelFile;
	copy->_generateCodeListing = _generateCodeListing;
	copy->_symbolsAreCaseSensitive = _symbolsAreCaseSensitive;
	copy->_isActive = NO; // this is important, prevents having more than one active target at a time
	
	return copy;
}
#pragma mark NSCoding
- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	for (WCBuildDefine *define in [coder decodeObjectForKey:kWCBuildTargetDefinesKey])
		[self addObjectToDefines:define];

	for (WCIncludeDirectory *include in [coder decodeObjectForKey:kWCBuildTargetIncludeDirectoriesKey])
		[self addObjectToIncludeDirectories:include];

	_inputFileAlias = [[coder decodeObjectForKey:@"inputFileAlias"] retain];
	
	_outputName = [[coder decodeObjectForKey:kWCBuildTargetOutputNameKey] copy];
	_outputType = [[coder decodeObjectForKey:kWCBuildTargetOutputTypeKey] unsignedIntegerValue];
	
	_generateLabelFile = [[coder decodeObjectForKey:kWCBuildTargetGenerateLabelFileKey] boolValue];
	_generateCodeListing = [[coder decodeObjectForKey:kWCBuildTargetGenerateCodeListingKey] boolValue];
	_symbolsAreCaseSensitive = [[coder decodeObjectForKey:kWCBuildTargetLabelsAreCaseSensitiveKey] boolValue];
	_isActive = [[coder decodeObjectForKey:@"isActive"] boolValue];
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:[self defines] forKey:kWCBuildTargetDefinesKey];
	[coder encodeObject:[self includeDirectories] forKey:kWCBuildTargetIncludeDirectoriesKey];
	[coder encodeObject:[self inputFileAlias] forKey:@"inputFileAlias"];
	[coder encodeObject:[NSNumber numberWithUnsignedInteger:[self outputType]] forKey:kWCBuildTargetOutputTypeKey];
	[coder encodeObject:[self outputName] forKey:kWCBuildTargetOutputNameKey];
	[coder encodeObject:[NSNumber numberWithBool:[self generateLabelFile]] forKey:kWCBuildTargetGenerateLabelFileKey];
	[coder encodeObject:[NSNumber numberWithBool:[self generateCodeListing]] forKey:kWCBuildTargetGenerateCodeListingKey];
	[coder encodeObject:[NSNumber numberWithBool:[self symbolsAreCaseSensitive]] forKey:kWCBuildTargetLabelsAreCaseSensitiveKey];
	[coder encodeObject:[NSNumber numberWithBool:[self isActive]] forKey:@"isActive"];
	[super encodeWithCoder:coder];
}
#pragma mark WCPlistRepresentationProtocol
- (NSDictionary *)plistRepresentation {
	NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:[super plistRepresentation]];
	
	[dict setObject:[[self defines] valueForKeyPath:@"plistRepresentation"] forKey:kWCBuildTargetDefinesKey];
	[dict setObject:[[self includeDirectories] valueForKeyPath:@"plistRepresentation"] forKey:kWCBuildTargetIncludeDirectoriesKey];
	[dict setObject:[[self inputFileAlias] plistRepresentation] forKey:@"inputFileAlias"];
	[dict setObject:[NSNumber numberWithUnsignedInteger:[self outputType]] forKey:kWCBuildTargetOutputTypeKey];
	[dict setObject:[self outputName] forKey:kWCBuildTargetOutputNameKey];
	[dict setObject:[NSNumber numberWithBool:[self generateLabelFile]] forKey:kWCBuildTargetGenerateLabelFileKey];
	[dict setObject:[NSNumber numberWithBool:[self generateCodeListing]] forKey:kWCBuildTargetGenerateCodeListingKey];
	[dict setObject:[NSNumber numberWithBool:[self symbolsAreCaseSensitive]] forKey:kWCBuildTargetLabelsAreCaseSensitiveKey];
	[dict setObject:[NSNumber numberWithBool:[self isActive]] forKey:@"isActive"];
	
	return [[dict copy] autorelease];
}
#pragma mark *** Public Methods ***
+ (id)buildTargetWithOutputType:(WCBuildTargetOutputType)type outputName:(NSString *)outputName; {
	return [[[[self class] alloc] initWithOutputType:type outputName:outputName] autorelease];
}
- (id)initWithOutputType:(WCBuildTargetOutputType)type outputName:(NSString *)outputName; {
	if (!(self = [super initWithName:NSLocalizedString(@"New Target", @"new target default name")]))
		return nil;
	
	_outputType = type;
	_outputName = [outputName retain];
	
	return self;
}
#pragma mark NSKeyValueCoding
- (NSUInteger)countOfDefines; {
	return [_defines count];
}
- (id)objectInDefinesAtIndex:(NSUInteger)index; {
	return [_defines objectAtIndex:index];
}
- (void)addObjectToDefines:(id)object; {
	[self insertObject:object inDefinesAtIndex:[self countOfDefines]];
}
- (void)insertObject:(id)object inDefinesAtIndex:(NSUInteger)index; {
	if (!_defines)
		_defines = [[NSMutableArray alloc] init];
	
	[_defines insertObject:object atIndex:index];
}
- (void)removeObjectFromDefinesAtIndex:(NSUInteger)index; {
	[_defines removeObjectAtIndex:index];
}

- (NSUInteger)countOfIncludeDirectories; {
	return [_includeDirectories count];
}
- (id)objectInIncludeDirectoriesAtIndex:(NSUInteger)index; {
	return [_includeDirectories objectAtIndex:index];
}
- (void)addObjectToIncludeDirectories:(id)object; {
	[self insertObject:object inIncludeDirectoriesAtIndex:[self countOfIncludeDirectories]];
}
- (void)insertObject:(id)object inIncludeDirectoriesAtIndex:(NSUInteger)index; {
	if (!_includeDirectories)
		_includeDirectories = [[NSMutableArray alloc] init];
	
	[_includeDirectories insertObject:object atIndex:index];
}
- (void)removeObjectFromIncludeDirectoriesAtIndex:(NSUInteger)index; {
	[_includeDirectories removeObjectAtIndex:index];
}
#pragma mark Accessors
@dynamic inputFileAlias;
- (WCAlias *)inputFileAlias {
	return _inputFileAlias;
}
- (void)setInputFileAlias:(WCAlias *)inputFileAlias {
	if (_inputFileAlias == inputFileAlias)
		return;
	
	[_inputFileAlias release];
	_inputFileAlias = inputFileAlias;
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCBuildTargetInputFileDidChangeNotification object:self];
}
@synthesize project=_project;
@dynamic outputName;
- (NSString *)outputName {
	if (_outputName && [_outputName length])
		return _outputName;
	
	// otherwise use the input file name
	return [[[[[self inputFileAlias] URL] path] lastPathComponent] stringByDeletingPathExtension];
}
- (void)setOutputName:(NSString *)outputName {
	if ([_outputName isEqualToString:outputName])
		return;
	
	[_outputName release];
	_outputName = [outputName copy];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCBuildTargetOutputNameDidChangeNotification object:self];
}
@dynamic outputType;
- (WCBuildTargetOutputType)outputType {
	return _outputType;
}
- (void)setOutputType:(WCBuildTargetOutputType)outputType {
	if (_outputType == outputType)
		return;
	
	_outputType = outputType;
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCBuildTargetOutputTypeDidChangeNotification object:self];
}
@synthesize includeDirectories=_includeDirectories;
@synthesize defines=_defines;
@synthesize generateLabelFile=_generateLabelFile;
@synthesize generateCodeListing=_generateCodeListing;
@dynamic symbolsAreCaseSensitive;
- (BOOL)symbolsAreCaseSensitive {
	return _symbolsAreCaseSensitive;
}
- (void)setSymbolsAreCaseSensitive:(BOOL)flag {
	if (_symbolsAreCaseSensitive == flag)
		return;
	
	_symbolsAreCaseSensitive = flag;
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCBuildTargetLabelsAreCaseSensitiveDidChangeNotification object:self];
}
@synthesize isActive=_isActive;
@dynamic mutableDefines;
- (NSMutableArray *)mutableDefines {
	if (!_defines)
		_defines = [[NSMutableArray alloc] init];
	
	return [self mutableArrayValueForKey:kWCBuildTargetDefinesKey];
}
@dynamic mutableIncludeDirectories;
- (NSMutableArray *)mutableIncludeDirectories {
	if (!_includeDirectories)
		_includeDirectories = [[NSMutableArray alloc] init];
	
	return [self mutableArrayValueForKey:kWCBuildTargetIncludeDirectoriesKey];
}
@dynamic outputExtension;
- (NSString *)outputExtension {
	if (!_outputTypesToFileExtensions) {
		_outputTypesToFileExtensions = [[NSDictionary alloc] initWithContentsOfURL:[[NSBundle mainBundle] URLForResource:@"BuildTargetOutputTypesToFileExtensions" withExtension:@"plist"]];
		
#ifdef DEBUG
		NSAssert(_outputTypesToFileExtensions != nil, @"failed to load target outputs to file extensions plist");
#endif
	}
	return [_outputTypesToFileExtensions objectForKey:[NSString stringWithFormat:@"%u",[self outputType]]];
}
@end
