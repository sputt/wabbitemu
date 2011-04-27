//
//  WECalculator.m
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WECalculator.h"
#import "WEApplicationDelegate.h"
#import "WCDefines.h"
#import "WELCDView.h"

#import <BWToolkitFramework/BWAnchoredButtonBar.h>


static NSString *const kWECalculatorErrorDomain = @"kWECalculatorErrorDomain";
static const NSInteger kWECalculatorCreatedMaxCalcs = 1001;
static const NSInteger kWECalculatorRomLoadFailed = 1002;

@interface WECalculator ()
- (NSString *)_stringForCalculatorModel:(WECalculatorModel)calculatorModel;
@end

@implementation WECalculator

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_statusString release];
	calc_slot_free(_calc);
	[super dealloc];
}

- (NSString *)windowNibName {
	return @"WECalculator";
}

- (void)windowWillClose:(NSNotification *)notification {
	
	[(WEApplicationDelegate *)[[NSApplication sharedApplication] delegate] removeLCDView:[self LCDView]];
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	[super windowControllerDidLoadNib:aController];
	
	[[self LCDView] setCalc:[self calc]];
	[[self LCDView] setIsWidescreen:([self calc]->model == TI_85 || [self calc]->model == TI_86)];
	[(WEApplicationDelegate *)[[NSApplication sharedApplication] delegate] addLCDView:[self LCDView]];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
	[[_statusTextField cell] setBackgroundStyle:NSBackgroundStyleLight];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError {
	LPCALC calc = calc_slot_new();
	
	if (calc == NULL) {
		if (outError != NULL)
			*outError = [NSError errorWithDomain:kWECalculatorErrorDomain code:kWECalculatorCreatedMaxCalcs userInfo:[NSDictionary dictionaryWithObjectsAndKeys:NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedDescriptionKey,NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedFailureReasonErrorKey,NSLocalizedString(@"Please close at least one open calculator and try again.", @"Please close at least one open calculator and try again."),NSLocalizedRecoverySuggestionErrorKey, nil]];
		return NO;
	}
	
	_calc = calc;
	
	return [self loadRom:absoluteURL error:outError];
}

- (BOOL)loadRom:(NSURL *)romURL error:(NSError **)outError {
	[self setIsRunning:NO];
	[self setIsLoadingRom:YES];
	
	NSString *path = [romURL path];
	LPCTSTR cPath = [path fileSystemRepresentation];
	BOOL loaded = rom_load([self calc], cPath);
	
	if (!loaded) {
		if (outError != NULL)
			*outError = [NSError errorWithDomain:kWECalculatorErrorDomain code:kWECalculatorRomLoadFailed userInfo:[NSDictionary dictionaryWithObjectsAndKeys:NSLocalizedString(@"The file could not be recognized as a rom or save state.", @"The file could not be recognized as a rom or save state."),NSLocalizedDescriptionKey,NSLocalizedString(@"The file could not be recognized as a rom or save state.", @"The file could not be recognized as a rom or save state."),NSLocalizedFailureReasonErrorKey,NSLocalizedString(@"Please try again with a different file.", @"Please try again with a different file."),NSLocalizedRecoverySuggestionErrorKey, nil]];
		
		[self setIsLoadingRom:NO];
		
		return NO;
	}
	
	if ([self windowForSheet] != nil) {
		[self setFileURL:romURL];
		[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[self fileURL]];
	}
	
	calc_turn_on([self calc]);
	
	[[self LCDView] setIsWidescreen:([self calc]->model == WECalculatorModelTI85 || [self calc]->model == WECalculatorModelTI86)];	
	
	[self setIsLoadingRom:NO];
	
	return YES;
}

- (void)updateStatusString; {
	if (![self isLoadingRom]) {
		[self setStatusString:[NSString stringWithFormat:NSLocalizedString(@"%@, FPS: %.2f", @"calculator model and fps status string"),[self _stringForCalculatorModel:[self model]],[self calc]->cpu.pio.lcd->ufps]];
	}
}

@synthesize LCDView=_LCDView;
@synthesize calc=_calc;
@dynamic isRunning;
- (BOOL)isRunning {
	return (_calc != NULL && _calc->running); 
}
- (void)setIsRunning:(BOOL)isRunning {
	if (_calc != NULL) {
		_calc->running = isRunning;
	}
}
@synthesize isLoadingRom=_isLoadingRom;
@synthesize statusString=_statusString;
@dynamic model;
- (WECalculatorModel)model {
	if (_calc == NULL)
		return NSNotFound;
	return (WECalculatorModel)[self calc]->model;
}
@synthesize connectionStatus=_connectionStatus;

- (IBAction)loadRom:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setAllowedFileTypes:[NSArray arrayWithObjects:@"rom",@"sav", nil]];
	[panel setPrompt:NS_LOCALIZED_STRING_LOAD];
	
	[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
		[panel orderOut:nil];
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		NSError *error;
		if (![self loadRom:[[panel URLs] lastObject] error:&error]) {
			if (error != NULL)
				[self presentError:error];
		}
	}];
}

- (IBAction)saveStateAs:(id)sender; {
	NSSavePanel *panel = [NSSavePanel savePanel];
	
	[panel setRequiredFileType:@"sav"];
	
	[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		SAVESTATE_t *savestate = SaveSlot([self calc]);
		
		if (savestate == NULL) {
			NSLog(@"savestate was NULL!");
			return;
		}
		
		NSString *path = [[panel URL] path];
		const char *cPath = [path fileSystemRepresentation];
		WriteSave(cPath, savestate, 0);
	}];
}
		 
- (NSString *)_stringForCalculatorModel:(WECalculatorModel)calculatorModel; {
	switch ([self calc]->model) {
		case WECalculatorModelTI73:
			return NSLocalizedString(@"TI-73", @"TI-73");
		case WECalculatorModelTI81:
			return NSLocalizedString(@"TI-81", @"TI-81");
		case WECalculatorModelTI82:
			return NSLocalizedString(@"TI-82", @"TI-82");
		case WECalculatorModelTI83:
			return NSLocalizedString(@"TI-83", @"TI-83");
		case WECalculatorModelTI83P:
			return NSLocalizedString(@"TI-83+", @"TI-83+");
		case WECalculatorModelTI83PSE:
			return NSLocalizedString(@"TI-83+SE", @"TI-83+SE");
		case WECalculatorModelTI84P:
			return NSLocalizedString(@"TI-84+", @"TI-84+");
		case WECalculatorModelTI84PSE:
			return NSLocalizedString(@"TI-84+SE", @"TI-84+SE");
		case WECalculatorModelTI85:
			return NSLocalizedString(@"TI-85", @"TI-85");
		case WECalculatorModelTI86:
			return NSLocalizedString(@"TI-86", @"TI-86");
		default:
			return nil;
	}
}
@end
