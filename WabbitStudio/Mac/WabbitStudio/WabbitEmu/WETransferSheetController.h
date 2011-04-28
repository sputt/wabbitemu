//
//  WETransferViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#include "savestate.h"


@class WECalculator;

@interface WETransferSheetController : NSWindowController {
@private
	IBOutlet NSProgressIndicator *_progressIndicator;
	
	__weak WECalculator *_calculator;
    NSMutableArray *_filePaths;
	NSPointerArray *_tiFiles;
	CGFloat _totalProgress;
	CGFloat _currentProgress;
	NSString *_statusString;
	SAVESTATE_t *_savestate;
	size_t _lastSentAmount;
	BOOL _knowsTotalProgress;
	BOOL _shouldAnimate;
}
@property (readonly,nonatomic) WECalculator *calculator;
@property (assign,nonatomic) CGFloat totalProgress;
@property (assign,nonatomic) CGFloat currentProgress;
@property (copy,nonatomic) NSString *statusString;
@property (assign,nonatomic) BOOL knowsTotalProgress;
@property (assign,nonatomic) BOOL shouldAnimate;
@property (assign) BOOL shouldCancel;

+ (void)transferFiles:(NSArray *)filePaths toCalculator:(WECalculator *)calculator;
+ (id)transferViewControllerWithFilePaths:(NSArray *)filePaths forCalculator:(WECalculator *)calculator;
- (id)initWithFilePaths:(NSArray *)filePaths calculator:(WECalculator *)calculator;
@end
