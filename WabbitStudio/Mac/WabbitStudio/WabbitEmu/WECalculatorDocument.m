//
//  WECalculatorDocument.m
//  WabbitEmu Beta
//
//  Created by William Towe on 4/25/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WECalculatorDocument.h"
#import "WEApplicationDelegate.h"
#import "WCDefines.h"
#import "WELCDView.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WEPreferencesController.h"
#import "RSCalculator.h"
#import "RSCalculatorSkinView.h"

#import <BWToolkitFramework/BWAnchoredButtonBar.h>


NSString *const kWECalculatorWillCloseNotification = @"kWECalculatorWillCloseNotification";

static NSString *const kWECalculatorErrorDomain = @"kWECalculatorErrorDomain";
static const NSInteger kWECalculatorCreatedMaxCalcs = 1001;
static const NSInteger kWECalculatorRomOrSavestateLoadFailed = 1002;

@interface WECalculatorDocument ()
- (NSString *)_stringForCalculatorModel:(RSCalculatorModel)calculatorModel;
@end

@implementation WECalculatorDocument

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
	return @"WECalculatorDocument";
}

- (void)windowWillClose:(NSNotification *)notification {	
	_isClosing = YES;
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWECalculatorWillCloseNotification object:self];
	
	[WEApplicationDelegate removeLCDView:[self LCDView]];
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	[super windowControllerDidLoadNib:aController];
	
	[aController setShouldCloseDocument:YES];
	
	[[self LCDView] setCalculator:[self calculator]];
	[[self LCDView] setIsWidescreen:([[self calculator] calc]->model == TI_85 || [[self calculator] calc]->model == TI_86)];
	[self updateStatusString];
	[self resetDisplaySize:nil];
	[WEApplicationDelegate addLCDView:[self LCDView]];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
	[[_statusTextField cell] setBackgroundStyle:NSBackgroundStyleLight];
	
	[self toggleSkinView:nil];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError {
	RSCalculator *calculator = [RSCalculator calculatorWithOwner:self breakpointSelector:@selector(handleBreakpoint)];
	
	if (calculator == nil) {
		if (outError != NULL)
			*outError = [NSError errorWithDomain:kWECalculatorErrorDomain code:kWECalculatorCreatedMaxCalcs userInfo:[NSDictionary dictionaryWithObjectsAndKeys:NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedDescriptionKey,NSLocalizedString(@"The maximum number of calculators have already been created.", @"The maximum number of calculators have already been created"),NSLocalizedFailureReasonErrorKey,NSLocalizedString(@"Please close at least one open calculator and try again.", @"Please close at least one open calculator and try again."),NSLocalizedRecoverySuggestionErrorKey, nil]];
		return NO;
	}
	
	_calculator = [calculator retain];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_calculatorModelDidChange:) name:kRSCalculatorModelDidChangeNotification object:[self calculator]];
	
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
	return [self windowForSheet];
}
@synthesize isDebugging=_isDebugging;
@synthesize hasSkin=_hasSkin;
@synthesize isBorderlessSkin=_isBorderlessSkin;

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
			return;
		}
		
		[[self LCDView] setIsWidescreen:([[self calculator] calc]->model == TI_85 || [[self calculator] calc]->model == TI_86)];
		[self updateStatusString];
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
	if ([self hasSkin])
		return;
	
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
	if ([self hasSkin])
		return;
	
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

- (IBAction)toggleSkinView:(id)sender; {
	BOOL useSkins = YES;
	BOOL useBorderlessSkin = YES;
	
	[self setHasSkin:useSkins];
	[self setIsBorderlessSkin:useBorderlessSkin];
	
	RSCalculatorSkinView *skinView = nil;
	for (NSView *view in [[[self windowForSheet] contentView] subviews]) {
		if ([view isKindOfClass:[RSCalculatorSkinView class]]) {
			skinView = (RSCalculatorSkinView *)view;
			break;
		}
	}
	
	NSUInteger baseWindowMask = (NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask);
	
	if (!useSkins) {
		if (skinView) {
			[_LCDView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
			
			// remove the view and resize the window frame
			[skinView removeFromSuperviewWithoutNeedingDisplay];
			
			[[self windowForSheet] setFrame:[[self windowForSheet] frameRectForContentRect:NSMakeRect([[self windowForSheet] frame].origin.x, [[self windowForSheet] frame].origin.y, [_LCDView frame].size.width, [_LCDView frame].size.height + [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge])] display:YES];
			
			[_LCDView setFrameOrigin:NSMakePoint(0.0, [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge])];
		}
		[[self windowForSheet] setStyleMask:baseWindowMask|NSResizableWindowMask];
	}
	else {
		if (useBorderlessSkin)
			[[self windowForSheet] setStyleMask:NSBorderlessWindowMask];
		else
			[[self windowForSheet] setStyleMask:baseWindowMask];
		
		NSImage *skin = [[self calculator] skinImage];
		
		[skinView removeFromSuperviewWithoutNeedingDisplay];
		
		skinView = [[[RSCalculatorSkinView alloc] initWithCalculator:[self calculator] frame:NSMakeRect(0, 0, [skin size].width, [skin size].height)] autorelease];
		
		[skinView setImageScaling:NSImageScaleNone];
		[skinView setAutoresizingMask:(NSViewMaxXMargin|NSViewMinYMargin)];
		
		[skinView setImage:skin];
		
		[_LCDView setFrameSize:NSMakeSize(kLCDWidth*2, kLCDHeight*2)];
		[_LCDView setAutoresizingMask:NSViewNotSizable];
		
		[[self windowForSheet] setFrame:[[self windowForSheet] frameRectForContentRect:NSMakeRect([[self windowForSheet] frame].origin.x, [[self windowForSheet] frame].origin.y, [skin size].width, [skin size].height + [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge])] display:YES animate:NO];
		
		[[[self windowForSheet] contentView] addSubview:skinView positioned:NSWindowBelow relativeTo:_LCDView];
		[skinView setFrameOrigin:NSMakePoint([skinView frame].origin.x, [skinView frame].origin.y + [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge])];
		
		if (useBorderlessSkin) {
			[[self windowForSheet] center];
			[[self windowForSheet] setBackgroundColor:[NSColor clearColor]];
			[[self windowForSheet] setOpaque:NO];
			[[self windowForSheet] setHasShadow:NO];
		}
		else {
			[[self windowForSheet] setBackgroundColor:[NSColor windowBackgroundColor]];
			[[self windowForSheet] setOpaque:YES];
			[[self windowForSheet] setHasShadow:YES];
		}
		
		NSImage *keymap = [[self calculator] keymapImage];
		if (keymap) {
			NSBitmapImageRep *bitmap = (NSBitmapImageRep *)[keymap bestRepresentationForRect:NSZeroRect context:nil hints:nil];
			if (bitmap) {
				NSSize size = [bitmap size];
				NSUInteger width = size.width, height = size.height;
				NSUInteger i, j;
				NSUInteger pixels[4];
				NSPoint point = NSZeroPoint;
				NSPoint endPoint = NSZeroPoint;
				
				for (i = 0; i < width; i++) {
					for (j = 0; j < height; j++) {
						[bitmap getPixel:pixels atX:i y:j];
						
						if (pixels[0] >= 200 && pixels[1] <= 200 && pixels[2] <= 200) {
							point.x = i;
							point.y = j;
							
							while (i < width && pixels[0] >= 200 && pixels[1] <= 200 && pixels[2] <= 200)
								[bitmap getPixel:pixels atX:i++ y:j];
							
							endPoint.x = i;
							endPoint.y = j;
							break;
						}
					}
					
					if (!NSEqualPoints(point, NSZeroPoint))
						break;
				}
				
				if (!NSEqualPoints(point, NSZeroPoint)) {
					[_LCDView setFrameOrigin:NSMakePoint(point.x, ([skin size].height + [[self windowForSheet] contentBorderThicknessForEdge:NSMinYEdge]) - (point.y + [_LCDView frame].size.height))];
					[_LCDView setFrameSize:NSMakeSize(endPoint.x-point.x, kLCDHeight*2)];
				}
			}
		}
	}
}

- (IBAction)step:(id)sender; {
	
}

- (IBAction)stepOver:(id)sender {
	
}

- (IBAction)stepOut:(id)sender {
	
}

- (NSString *)_stringForCalculatorModel:(RSCalculatorModel)calculatorModel; {
	switch (calculatorModel) {
		case RSCalculatorModelTI73:
			return NSLocalizedString(@"TI-73", @"TI-73");
		case RSCalculatorModelTI81:
			return NSLocalizedString(@"TI-81", @"TI-81");
		case RSCalculatorModelTI82:
			return NSLocalizedString(@"TI-82", @"TI-82");
		case RSCalculatorModelTI83:
			return NSLocalizedString(@"TI-83", @"TI-83");
		case RSCalculatorModelTI83P:
			return NSLocalizedString(@"TI-83+", @"TI-83+");
		case RSCalculatorModelTI83PSE:
			return NSLocalizedString(@"TI-83+SE", @"TI-83+SE");
		case RSCalculatorModelTI84P:
			return NSLocalizedString(@"TI-84+", @"TI-84+");
		case RSCalculatorModelTI84PSE:
			return NSLocalizedString(@"TI-84+SE", @"TI-84+SE");
		case RSCalculatorModelTI85:
			return NSLocalizedString(@"TI-85", @"TI-85");
		case RSCalculatorModelTI86:
			return NSLocalizedString(@"TI-86", @"TI-86");
		default:
			return nil;
	}
}

- (void)_calculatorModelDidChange:(NSNotification *)note {
	[self updateFPSString];
	[_LCDView setIsWidescreen:([[self calculator] model] == RSCalculatorModelTI85 || [[self calculator] model] == RSCalculatorModelTI86)];
	[self toggleSkinView:nil];
}

@end
