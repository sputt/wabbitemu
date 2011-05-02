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
#import "NSUserDefaults+WCExtensions.h"
#import "WEPreferencesController.h"
#import "RSCalculator.h"

#import <BWToolkitFramework/BWAnchoredButtonBar.h>


NSString *const kWECalculatorWillCloseNotification = @"kWECalculatorWillCloseNotification";

static NSString *const kWECalculatorErrorDomain = @"kWECalculatorErrorDomain";
static const NSInteger kWECalculatorCreatedMaxCalcs = 1001;
static const NSInteger kWECalculatorRomOrSavestateLoadFailed = 1002;

@interface WECalculator ()
- (NSString *)_stringForCalculatorModel:(WECalculatorModel)calculatorModel;
@end

@implementation WECalculator

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_statusString release];
	[_FPSString release];
	[_calculator release];
	[super dealloc];
}

- (NSString *)windowNibName {
	return @"WECalculator";
}

- (void)windowWillClose:(NSNotification *)notification {
	_isClosing = YES;
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWECalculatorWillCloseNotification object:self];
	
	[WEApplicationDelegate removeLCDView:[self LCDView]];
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	[super windowControllerDidLoadNib:aController];
	
	[[self LCDView] setCalculator:[self calculator]];
	[[self LCDView] setIsWidescreen:([[self calculator] calc]->model == TI_85 || [[self calculator] calc]->model == TI_86)];
	[self resetDisplaySize:nil];
	[WEApplicationDelegate addLCDView:[self LCDView]];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
	[[_statusTextField cell] setBackgroundStyle:NSBackgroundStyleLight];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError {
	RSCalculator *calculator = [RSCalculator calculatorWithOwner:self breakpointSelector:@selector(handleBreakpoint)];
	
	if (calculator == nil) {
		if (outError != NULL)
			*outError = [NSError errorWithDomain:kWECalculatorErrorDomain code:kWECalculatorCreatedMaxCalcs userInfo:[NSDictionary dictionaryWithObjectsAndKeys:NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedDescriptionKey,NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedFailureReasonErrorKey,NSLocalizedString(@"Please close at least one open calculator and try again.", @"Please close at least one open calculator and try again."),NSLocalizedRecoverySuggestionErrorKey, nil]];
		return NO;
	}
	
	_calculator = [calculator retain];
	
	return [[self calculator] loadRomOrSavestate:[absoluteURL path] error:outError];
}

- (void)updateFPSString; {
	if (![[self calculator] isBusy]) {
		[self setFPSString:[NSString stringWithFormat:NSLocalizedString(@"FPS: %.2f", @"FPS format string"),[[self calculator] calc]->cpu.pio.lcd->ufps]];
	}
}

- (void)updateStatusString {
	if ([[self calculator] calc] != NULL) {
		[self setStatusString:[self _stringForCalculatorModel:[[self calculator] model]]];
	}
}

@synthesize LCDView=_LCDView;
@synthesize calculator=_calculator;
@synthesize statusString=_statusString;
@synthesize FPSString=_FPSString;

@dynamic calculatorWindow;
- (NSWindow *)calculatorWindow {
	return [[[self windowControllers] objectAtIndex:0] window];
}
@dynamic isDebugging;
- (BOOL)isDebugging {
	return _isDebugging;
}
- (void)setIsDebugging:(BOOL)isDebugging {
	if (_isDebugging == isDebugging)
		return;
	
	_isDebugging = isDebugging;
}

- (IBAction)loadRom:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setAllowedFileTypes:[NSArray arrayWithObjects:@"rom",@"sav", nil]];
	[panel setPrompt:NS_LOCALIZED_STRING_LOAD];
	
	[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
		[panel orderOut:nil];
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		NSError *error;
		if (![[self calculator] loadRomOrSavestate:[[[panel URLs] lastObject] path] error:&error]) {
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
		
		SAVESTATE_t *savestate = SaveSlot([[self calculator] calc]);
		
		if (savestate == NULL) {
			NSLog(@"savestate was NULL!");
			return;
		}
		
		WriteSave([[[panel URL] path] fileSystemRepresentation], savestate, 0);
	}];
}

- (IBAction)reloadCurrentRomOrSavestate:(id)sender; {
	[[self calculator] loadRomOrSavestate:[[self fileURL] path] error:NULL];
}
- (IBAction)resetCalculator:(id)sender; {
	[[self calculator] setIsRunning:NO];
	calc_reset([[self calculator] calc]);
	calc_turn_on([[self calculator] calc]);
	[[self calculator] setIsRunning:YES];
}

- (IBAction)toggleLCDSize:(id)sender; {
	CGFloat bottomBorderHeight = [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge];
	NSSize lcdSize = NSZeroSize;
	CGFloat baseWidth = ([[self LCDView] isWidescreen])?(kLCDWidescreenWidth*2):(kLCDWidth*2);
	CGFloat baseHeight = (kLCDHeight*2);
	
	switch ([sender tag]) {
		case 0:
			lcdSize = NSMakeSize(baseWidth, baseHeight);
			break;
		case 1:
			lcdSize = NSMakeSize(baseWidth*2, baseHeight*2);
			break;
		case 2:
			lcdSize = NSMakeSize(baseWidth*4, baseHeight*4);
			break;
		default:
			break;
	}	
	
	NSRect currentRect = [[self windowForSheet] frame];
	NSRect newRect = [[self windowForSheet] frameRectForContentRect:NSMakeRect(NSMinX(currentRect), NSMinY(currentRect), lcdSize.width, lcdSize.height+bottomBorderHeight)];
	
	if (NSHeight(currentRect) < NSHeight(newRect))
		newRect.origin.y -= NSHeight(newRect) - NSHeight(currentRect);
	else if (NSHeight(currentRect) > NSHeight(newRect))
		newRect.origin.y += NSHeight(currentRect) - NSHeight(newRect);
	else
		return;
	
	[[self windowForSheet] setFrame:newRect display:YES animate:YES];
}

- (IBAction)resetDisplaySize:(id)sender; {
	CGFloat bottomBorderHeight = [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge];
	CGFloat baseWidth = ([[self LCDView] isWidescreen])?(kLCDWidescreenWidth*2):(kLCDWidth*2);
	CGFloat baseHeight = (kLCDHeight*2);
	NSSize lcdSize = NSMakeSize(baseWidth, baseHeight);
	NSRect currentRect = [[self windowForSheet] frame];
	NSRect newRect = [[self windowForSheet] frameRectForContentRect:NSMakeRect(NSMinX(currentRect), NSMinY(currentRect), lcdSize.width, lcdSize.height+bottomBorderHeight)];
	
	if (NSHeight(currentRect) < NSHeight(newRect))
		newRect.origin.y -= NSHeight(newRect) - NSHeight(currentRect);
	else if (NSHeight(currentRect) > NSHeight(newRect))
		newRect.origin.y += NSHeight(currentRect) - NSHeight(newRect);
	
	[[self windowForSheet] setFrame:newRect display:YES animate:YES];
}

- (IBAction)step:(id)sender; {
	
}

- (IBAction)stepOver:(id)sender {
	
}

- (IBAction)stepOut:(id)sender {
	
}

- (NSString *)_stringForCalculatorModel:(WECalculatorModel)calculatorModel; {
	switch (calculatorModel) {
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
