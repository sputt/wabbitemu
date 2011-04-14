//
//  WCBuildTarget.h
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"

enum WCBuildTargetOutputType {
	WCBuildTargetOutputTypeBinary = 0,
	WCBuildTargetOutputType73 = 1,
	WCBuildTargetOutputType82 = 2,
	WCBuildTargetOutputType83 = 3,
	WCBuildTargetOutputType83Plus = 4,
	WCBuildTargetOutputType83PlusApp = 5,
	WCBuildTargetOutputType85 = 6,
	WCBuildTargetOutputType86 = 7
};
typedef NSUInteger WCBuildTargetOutputType;

extern NSString *const kWCBuildTargetLabelsAreCaseSensitiveDidChangeNotification;
extern NSString *const kWCBuildTargetNameDidChangeNotification;
extern NSString *const kWCBuildTargetInputFileDidChangeNotification;
extern NSString *const kWCBuildTargetOutputNameDidChangeNotification;
extern NSString *const kWCBuildTargetOutputTypeDidChangeNotification;

@class WCFile,WCProject,WCAlias;

@interface WCBuildTarget : WCTreeNode <NSCoding,NSCopying,NSMutableCopying> {
@private
	__weak WCProject *_project;
    WCBuildTargetOutputType _outputType;
	NSString *_outputName;
	WCAlias *_inputFileAlias;
	NSMutableArray *_includeDirectories;
	NSMutableArray *_defines;
	BOOL _generateCodeListing;
	BOOL _generateLabelFile;
	BOOL _symbolsAreCaseSensitive;
	BOOL _isActive;
}

@property (assign,nonatomic) WCProject *project;
@property (assign,nonatomic) WCBuildTargetOutputType outputType;
@property (copy,nonatomic) NSString *outputName;
@property (retain,nonatomic) WCAlias *inputFileAlias;
@property (readonly,nonatomic) NSArray *includeDirectories;
@property (readonly,nonatomic) NSArray *defines;
@property (readonly,nonatomic) NSMutableArray *mutableDefines;
@property (readonly,nonatomic) NSMutableArray *mutableIncludeDirectories;
@property (assign,nonatomic) BOOL generateCodeListing;
@property (assign,nonatomic) BOOL generateLabelFile;
@property (assign,nonatomic) BOOL symbolsAreCaseSensitive;
@property (assign,nonatomic) BOOL isActive;
@property (readonly,nonatomic) NSString *outputExtension;

+ (id)buildTargetWithOutputType:(WCBuildTargetOutputType)type outputName:(NSString *)outputName;
- (id)initWithOutputType:(WCBuildTargetOutputType)type outputName:(NSString *)outputName;

- (NSUInteger)countOfIncludeDirectories;
- (id)objectInIncludeDirectoriesAtIndex:(NSUInteger)index;
- (void)addObjectToIncludeDirectories:(id)object;
- (void)insertObject:(id)object inIncludeDirectoriesAtIndex:(NSUInteger)index;
- (void)removeObjectFromIncludeDirectoriesAtIndex:(NSUInteger)index;

- (NSUInteger)countOfDefines;
- (id)objectInDefinesAtIndex:(NSUInteger)index;
- (void)addObjectToDefines:(id)object;
- (void)insertObject:(id)object inDefinesAtIndex:(NSUInteger)index;
- (void)removeObjectFromDefinesAtIndex:(NSUInteger)index;
@end
