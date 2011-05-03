//
//  WETransferViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WETransferSheetController.h"
#import "NSResponder+WCExtensions.h"
#import "WETransferFile.h"
#import "WCDefines.h"
#import "RSCalculator.h"


@interface WETransferSheetController ()
- (id)_initWithFilePaths:(NSArray *)filePaths calculator:(RSCalculator *)calculator;
- (void)_transferRomsAndSavestates;
- (void)_setupTransferProgramsAndApps;
- (void)_transferProgramsAndApps;
@end

@implementation WETransferSheetController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_currentFile = nil;
	_calculator = nil;
	FreeSave(_savestate);
	[_statusString release];
	[_programsAndApps release];
	[_romsAndSavestates release];
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WETransferSheet";
}

- (void)windowDidLoad {
	[super windowDidLoad];
	
	[self _transferProgramsAndApps];
}

+ (void)transferFiles:(NSArray *)filePaths toCalculator:(RSCalculator *)calculator; {
	[self transferFiles:filePaths toCalculator:calculator runAfterTransfer:NO];
}

+ (void)transferFiles:(NSArray *)filePaths toCalculator:(RSCalculator *)calculator runAfterTransfer:(BOOL)runAfterTransfer; {
	WETransferSheetController *controller = [[[self class] alloc] _initWithFilePaths:[self validateFilePaths:filePaths] calculator:calculator];
	
	[controller setRunProgramOrAppAfterTransfer:runAfterTransfer];
	[controller _transferRomsAndSavestates];
	[controller _setupTransferProgramsAndApps];
}

- (id)_initWithFilePaths:(NSArray *)filePaths calculator:(RSCalculator *)calculator; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_calculator = calculator;
	_programsAndApps = [[NSMutableArray alloc] init];
	_romsAndSavestates = [[NSMutableArray alloc] init];
	
	for (NSString *filePath in filePaths) {
		WETransferFile *file = [WETransferFile transferFileWithPath:filePath];
		
		switch ([file type]) {
			case WETransferFileTypeRom:
			case WETransferFileTypeSavestate:
				[_romsAndSavestates addObject:file];
				break;
			case WETransferFileTypeGroup:
			case WETransferFileTypeFlash:
			case WETransferFileTypeVar:
				[_programsAndApps addObject:file];
				break;
			default:
				break;
		}
	}
	
	return self;
}

+ (NSArray *)validateFilePaths:(NSArray *)filePaths; {
	NSMutableArray *validFilePaths = [NSMutableArray arrayWithCapacity:[filePaths count]];
	
	for (NSString *path in filePaths) {
		NSString *UTI = [[NSWorkspace sharedWorkspace] typeOfFile:path error:NULL];
		
		if ([UTI isEqualToString:kWECalculatorProgramUTI] ||
			[UTI isEqualToString:kWECalculatorApplicationUTI] ||
			[UTI isEqualToString:kWECalculatorSavestateUTI] ||
			[UTI isEqualToString:kWECalculatorRomUTI])
			[validFilePaths addObject:path];
	}
	return [[validFilePaths copy] autorelease];
}

@synthesize calculator=_calculator;
@synthesize statusString=_statusString;
@synthesize totalSize=_totalSize;
@synthesize currentProgress=_currentProgress;
@synthesize currentFile=_currentFile;
@synthesize runProgramOrAppAfterTransfer=_runProgramOrAppAfterTransfer;

- (void)_transferRomsAndSavestates; {
	if ([_romsAndSavestates count] == 0)
		return;
	else if ([_romsAndSavestates count] == 1) {
		NSError *error;
		if (![[self calculator] loadRomOrSavestate:[[_romsAndSavestates objectAtIndex:0] path] error:&error])
			[[[self calculator] owner] presentError:error];
	}
	else {
		NSError *error;
		if (![[self calculator] loadRomOrSavestate:[[_romsAndSavestates objectAtIndex:0] path] error:&error])
			[[[self calculator] owner] presentError:error];
		
		[_romsAndSavestates removeObjectAtIndex:0];
		
		for (WETransferFile *file in _romsAndSavestates) {
			if (![[NSDocumentController sharedDocumentController] openDocumentWithContentsOfURL:[NSURL fileURLWithPath:[file path]] display:YES error:&error])
				[[[self calculator] owner] presentError:error];
		}
	}
}

- (void)_setupTransferProgramsAndApps; {
	if ([_programsAndApps count] == 0) {
		[self autorelease];
		return;
	}
	
	[[NSApplication sharedApplication] beginSheet:[self window] modalForWindow:[[[self calculator] owner] windowForSheet] modalDelegate:self didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

- (void)_transferProgramsAndApps; {
	[_progressIndicator startAnimation:nil];
	
	CGFloat totalSize = 0;
	for (WETransferFile *file in _programsAndApps)
		totalSize += [file size];
	
	[self setTotalSize:totalSize];
	
	[_progressIndicator setIndeterminate:NO];
	
	[[self calculator] setIsActive:NO];
	[[self calculator] setIsRunning:NO];
	
	NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:1.0/10 target:self selector:@selector(_timerFired:) userInfo:nil repeats:YES];
	
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		LPCALC calc = [[self calculator] calc];
		
		for (WETransferFile *file in _programsAndApps) {
			dispatch_sync(dispatch_get_main_queue(), ^{
				[self setCurrentFile:file];
			});
			
			TIFILE_t *tifile = [file tifile];
			
			calc->cpu.pio.link->vlink_send = 0;
			
			link_send_var(&calc->cpu, tifile, SEND_CUR); 
		}
		
		dispatch_async(dispatch_get_main_queue(), ^{
			[timer invalidate];
			[[self window] orderOut:nil];
			[[NSApplication sharedApplication] endSheet:[self window]];
		});
		
		[pool drain];
	});
}

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	[[self calculator] setIsActive:YES];
	[[self calculator] setIsRunning:YES];
	
	if ([self runProgramOrAppAfterTransfer]) {
#ifdef DEBUG
		NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
		calc_run_seconds([[self calculator] calc], 1.0);
		// 2nd
		[[self calculator] simulateKeyPress:48];
		// 0 for catalog
		[[self calculator] simulateKeyPress:29];
		// down
		[[self calculator] simulateKeyPress:125];
		[[self calculator] simulateKeyPress:125];
		[[self calculator] simulateKeyPress:125];
		[[self calculator] simulateKeyPress:125];
		[[self calculator] simulateKeyPress:125];
		[[self calculator] simulateKeyPress:125];
		// return
		[[self calculator] simulateKeyPress:36];
		// programs
		[[self calculator] simulateKeyPress:38];
		// return
		[[self calculator] simulateKeyPress:36];
		// return
		[[self calculator] simulateKeyPress:36 lastKeyPressInSeries:YES];
	}
}

- (void)_timerFired:(NSTimer *)timer {
	[self setStatusString:[NSString stringWithFormat:NSLocalizedString(@"Transferring \"%@\" (%lu of %lu)\u2026", @"transfer sheet status string format"),[[[self currentFile] path] lastPathComponent],[_programsAndApps indexOfObject:[self currentFile]]+1,[_programsAndApps count]]];
	
	CGFloat currentFileProgress = ABS([[self calculator] calc]->cpu.pio.link->vlink_send - [[self currentFile] currentProgress]);
	
	if (currentFileProgress >= [[self currentFile] size]) {
		[[self currentFile] setCurrentProgress:0];
		return;
	}
	
	[self setCurrentProgress:[self currentProgress] + currentFileProgress];
	
	[[self currentFile] setCurrentProgress:[[self calculator] calc]->cpu.pio.link->vlink_send];
}
@end
