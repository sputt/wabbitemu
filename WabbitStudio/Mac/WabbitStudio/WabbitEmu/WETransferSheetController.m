//
//  WETransferViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WETransferSheetController.h"
#import "WECalculator.h"
#import "NSResponder+WCExtensions.h"


static void tiFilesRelinquishFunction(const void *item, NSUInteger (*size)(const void *item)) {
	FreeTiFile((TIFILE_t *)item);
}

@interface WETransferSheetController ()
- (void)_transferFiles;
@end

@implementation WETransferSheetController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_calculator = nil;
	FreeSave(_savestate);
	[_statusString release];
	[_filePaths release];
	[_tiFiles release];
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WETransferSheet";
}

- (void)windowDidLoad {
	[super windowDidLoad];
	
	[self _transferFiles];
}

+ (void)transferFiles:(NSArray *)filePaths toCalculator:(WECalculator *)calculator; {
	WETransferSheetController *controller = [[[self class] alloc] initWithFilePaths:filePaths calculator:calculator];
	
	[[NSApplication sharedApplication] beginSheet:[controller window] modalForWindow:[calculator windowForSheet] modalDelegate:controller didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

+ (id)transferViewControllerWithFilePaths:(NSArray *)filePaths forCalculator:(WECalculator *)calculator; {
	return [[[[self class] alloc] initWithFilePaths:filePaths calculator:calculator] autorelease];
}
- (id)initWithFilePaths:(NSArray *)filePaths calculator:(WECalculator *)calculator; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_calculator = calculator;
	_filePaths = [filePaths mutableCopy];
	
	return self;
}

@synthesize shouldAnimate=_shouldAnimate;
@synthesize knowsTotalProgress=_knowsTotalProgress;
@synthesize calculator=_calculator;
@synthesize statusString=_statusString;
@synthesize totalProgress=_totalProgress;
@synthesize currentProgress=_currentProgress;

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	[[self calculator] setIsActive:YES];
	[[self calculator] setIsRunning:YES];
}

- (void)_transferFiles; {
	[[self calculator] setIsActive:NO];
	[[self calculator] setIsRunning:NO];
	[self setShouldAnimate:YES];
	
	_savestate = SaveSlot([[self calculator] calc]);
	
	NSPointerFunctions *functions = [[[NSPointerFunctions alloc] initWithOptions:NSPointerFunctionsOpaqueMemory|NSPointerFunctionsOpaquePersonality] autorelease];
	[functions setRelinquishFunction:&tiFilesRelinquishFunction];
	
	_tiFiles = [[NSPointerArray alloc] initWithPointerFunctions:functions];
	[_tiFiles setCount:[_filePaths count]];
	
	NSUInteger totalSize = 0;
	for (NSUInteger fIndex = 0; fIndex < [_filePaths count]; fIndex++) {
		NSString *path = [_filePaths objectAtIndex:fIndex];
		TIFILE_t *tifile = newimportvar([path fileSystemRepresentation]);
		
		if (tifile == NULL)
			continue;
		
		[_tiFiles insertPointer:tifile atIndex:fIndex];
		
		switch (tifile->type) {
			case FLASH_TYPE:
				for (u_int16_t i = 0; i < 256; i++) {
					totalSize += tifile->flash->pagesize[i];
				}
				break;
			case GROUP_TYPE:
			case VAR_TYPE:
				totalSize += tifile->var->length;
				break;
			default:
				break;
		}
	}
	
#ifdef DEBUG
	NSLog(@"total size %lu",totalSize);
#endif
	
	[self setTotalProgress:totalSize];
	[self setKnowsTotalProgress:YES];
	
	NSTimer *progressTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/30 target:self selector:@selector(_timerFired:) userInfo:nil repeats:YES];
	
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];		
		NSMutableArray *filePaths = _filePaths;
		NSPointerArray *tifiles = _tiFiles;
		CPU_t *cpu = &[[self calculator] calc]->cpu;
		
		for (NSUInteger fIndex = 0; fIndex < [filePaths count]; fIndex++) {
			NSString *path = [filePaths objectAtIndex:fIndex];
			
			dispatch_async(dispatch_get_main_queue(), ^{
				[self setStatusString:[NSString stringWithFormat:NSLocalizedString(@"Transferring %@\u2026", @"transferring file name with ellipsis"),[path lastPathComponent]]];
			});

			TIFILE_t *tifile = [tifiles pointerAtIndex:fIndex];
			
			cpu->pio.link->vlink_send = 0;
			_lastSentAmount = 0;
			
			link_send_var(cpu, tifile, SEND_CUR);
		}
		
		dispatch_async(dispatch_get_main_queue(), ^{
			[progressTimer invalidate];
			[[self window] close];
			[[NSApplication sharedApplication] endSheet:[self window] returnCode:NSOKButton];
		});
		
		[pool drain];
	});
}

- (void)_timerFired:(NSTimer *)timer {
#ifdef DEBUG
	NSLog(@"amount sent %lu",[[self calculator] calc]->cpu.pio.link->vlink_send);
#endif
	if ([[self calculator] calc]->cpu.pio.link->vlink_send < [self totalProgress]) {
		[self setCurrentProgress:[self currentProgress] + ((CGFloat)[[self calculator] calc]->cpu.pio.link->vlink_send - _lastSentAmount)];
		_lastSentAmount = [[self calculator] calc]->cpu.pio.link->vlink_send;
	}
	else
		[self setKnowsTotalProgress:NO];
}
@end
