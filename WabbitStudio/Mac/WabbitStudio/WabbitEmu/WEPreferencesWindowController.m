//
//  WEPreferencesWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreferencesWindowController.h"
#import "WECalculator.h"
#import "WEPreviewLCDView.h"
#import "WEApplicationDelegate.h"
#import "WCDefines.h"
#import "RSCalculator.h"


@implementation WEPreferencesWindowController

+ (NSString *)nibName {
	return @"WEPreferencesWindow";
}

- (void)setupToolbar {
	[self addView:_general label:NSLocalizedString(@"General", @"General") image:[NSImage imageNamed:NSImageNamePreferencesGeneral]];
	[self addView:_calculator label:NSLocalizedString(@"Calculator", @"Calculator") image:[NSImage imageNamed:@"Hardware"]];
	[self addView:_advanced label:NSLocalizedString(@"Advanced", @"Advanced")];
}

- (void)windowWillLoad {
	[super windowWillLoad];
	
	[[NSDocumentController sharedDocumentController] addObserver:self forKeyPath:@"currentDocument" options:NSKeyValueObservingOptionNew context:(void *)self];
}

- (void)windowDidLoad { 
	[super windowDidLoad];
	
	[[self window] setDelegate:self];
	
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	if ([[[NSDocumentController sharedDocumentController] documents] count] > 0) {
		WECalculator *calculator = [[NSDocumentController sharedDocumentController] currentDocument];
		
		if (calculator == nil)
			calculator = [[[NSDocumentController sharedDocumentController] documents] lastObject];
		
		[_LCDView setCalculator:[calculator calculator]];
		[WEApplicationDelegate addLCDView:_LCDView];
	}
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_calculatorWillClose:) name:kWECalculatorWillCloseNotification object:nil];
}

- (void)windowWillClose:(NSNotification *)notification {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_LCDView setCalculator:nil];
	[WEApplicationDelegate removeLCDView:_LCDView];
}

- (void)windowDidBecomeMain:(NSNotification *)notification {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	if ([[[NSDocumentController sharedDocumentController] documents] count] > 0) {
		WECalculator *calculator = [[NSDocumentController sharedDocumentController] currentDocument];
		
		if (calculator == nil)
			calculator = [[[NSDocumentController sharedDocumentController] documents] lastObject];
		
		[_LCDView setCalculator:[calculator calculator]];
		[WEApplicationDelegate addLCDView:_LCDView];
	}
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([keyPath isEqualToString:@"currentDocument"] && (id)context == self) {
		if ([[[NSDocumentController sharedDocumentController] documents] count] > 0) {
			WECalculator *calculator = [[NSDocumentController sharedDocumentController] currentDocument];
			
			if (calculator == nil)
				calculator = [[[NSDocumentController sharedDocumentController] documents] lastObject];
			
			[_LCDView setCalculator:[calculator calculator]];
			[WEApplicationDelegate addLCDView:_LCDView];
		}
	}
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (void)_calculatorWillClose:(NSNotification *)note {
	if ([[note object] calculator] == [_LCDView calculator]) {
		[_LCDView setCalculator:nil];
		[WEApplicationDelegate removeLCDView:_LCDView];
		
	}
}
@end
