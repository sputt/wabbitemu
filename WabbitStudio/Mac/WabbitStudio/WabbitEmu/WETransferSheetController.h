//
//  WETransferViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#include "savestate.h"


@class WECalculator,WETransferFile;

@interface WETransferSheetController : NSWindowController {
@private
	IBOutlet NSProgressIndicator *_progressIndicator;
	
	__weak WECalculator *_calculator;
    NSMutableArray *_romsAndSavestates;
	NSMutableArray *_programsAndApps;
	CGFloat _totalSize;
	CGFloat _currentProgress;
	WETransferFile *_currentFile;
	NSString *_statusString;
	SAVESTATE_t *_savestate;
	
	BOOL _runProgramOrAppAfterTransfer;
}
@property (readonly,nonatomic) WECalculator *calculator;
@property (assign,nonatomic) CGFloat totalSize;
@property (assign,nonatomic) CGFloat currentProgress;
@property (copy,nonatomic) NSString *statusString;
@property (assign,nonatomic) WETransferFile *currentFile;
@property (assign,nonatomic) BOOL runProgramOrAppAfterTransfer;

+ (void)transferFiles:(NSArray *)filePaths toCalculator:(WECalculator *)calculator;
+ (void)transferFiles:(NSArray *)filePaths toCalculator:(WECalculator *)calculator runAfterTransfer:(BOOL)runAfterTransfer;
+ (NSArray *)validateFilePaths:(NSArray *)filePaths;
@end
