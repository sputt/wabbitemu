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

static NSString *const kWECalculatorErrorDomain = @"kWECalculatorErrorDomain";
static const NSInteger kWECalculatorCreatedMaxCalcs = 1001;
static const NSInteger kWECalculatorRomLoadFailed = 1002;


@implementation WECalculator

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif

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
	[(WEApplicationDelegate *)[[NSApplication sharedApplication] delegate] addLCDView:[self LCDView]];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	LPCALC calc = calc_slot_new();
	
	if (calc == NULL) {
		if (outError != NULL) {
			*outError = [NSError errorWithDomain:kWECalculatorErrorDomain code:kWECalculatorCreatedMaxCalcs userInfo:[NSDictionary dictionaryWithObjectsAndKeys:NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedDescriptionKey,NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedFailureReasonErrorKey,NSLocalizedString(@"Please close at least one open calculator and try again.", @"Please close at least one open calculator and try again."),NSLocalizedRecoverySuggestionErrorKey, nil]];
		}
		return NO;
	}
	
	_calc = calc;
	
	return [self loadRom:absoluteURL error:outError];
}

- (BOOL)loadRom:(NSURL *)romURL error:(NSError **)outError {
	[self setIsRunning:FALSE];
	
	NSString *path = [romURL path];
	LPCTSTR cPath = [path fileSystemRepresentation];
	BOOL loaded = rom_load([self calc], cPath);
	
	if (!loaded) {
		if (outError != NULL) {
			*outError = [NSError errorWithDomain:kWECalculatorErrorDomain code:kWECalculatorRomLoadFailed userInfo:[NSDictionary dictionaryWithObjectsAndKeys:NSLocalizedString(@"The file could not be recognized as a rom or save state.", @"The file could not be recognized as a rom or save state."),NSLocalizedDescriptionKey,NSLocalizedString(@"Please try again with a different file.", @"Please try again with a different file."),NSLocalizedRecoverySuggestionErrorKey, nil]];
		}
		return NO;
	}
	
	if ([self windowForSheet] != nil) {
		//[self setFileURL:romURL];
		//[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[self fileURL]];
		//[[self windowForSheet] makeFirstResponder:[self LCDView]];
	}
	
	//[self setIsRunning:TRUE];
	calc_turn_on([self calc]);
	
	return YES;
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

- (IBAction)loadRom:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setAllowedFileTypes:[NSArray arrayWithObjects:@"rom",@"sav", nil]];
	[panel setPrompt:NS_LOCALIZED_STRING_LOAD];
	
	[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
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
@end
