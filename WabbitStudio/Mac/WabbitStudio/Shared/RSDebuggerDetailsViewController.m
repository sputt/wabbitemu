//
//  RSDebuggerDetailsViewController.m
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerDetailsViewController.h"
#import "NSViewController+RSExtensions.h"
#import "WCDefines.h"


@interface RSDebuggerDetailsViewController ()

@end

@implementation RSDebuggerDetailsViewController

- (void)dealloc {
	[_calculator release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"RSDebuggerDetailsView";
}

- (void)loadView {
	[super loadView];
	
	[_animatingOutlineView setDelegate:self];
	
	[_animatingOutlineView addView:_registersView withImage:[NSImage imageNamed:@"Registers16x16"] label:NSLocalizedString(@"Registers", @"registers detail view label") expanded:YES];
	[_animatingOutlineView addView:_flagsView withImage:[NSImage imageNamed:@"Flags16x16"] label:NSLocalizedString(@"Flags", @"flags detail view label") expanded:YES];
	[_animatingOutlineView addView:_cpuView withImage:[NSImage imageNamed:@"CPU16x16"] label:NSLocalizedString(@"CPU", @"cpu detail view label") expanded:NO];
	[_animatingOutlineView addView:_memoryMapView withImage:[NSImage imageNamed:@"MemoryMap16x16"] label:NSLocalizedString(@"Memory Map", @"memory map detail view label") expanded:NO];
	[_animatingOutlineView addView:_interruptsView withImage:[NSImage imageNamed:@"Interrupts16x16"] label:NSLocalizedString(@"Interrupts", @"interrupts detail view label") expanded:NO];
	[_animatingOutlineView addView:_displayView withImage:[NSImage imageNamed:@"Display16x16"] label:NSLocalizedString(@"Display", @"display detail view label") expanded:NO];
	
	//[(NSScrollView *)[self view] setDocumentView:_animatingOutlineView];
}

- (CGFloat)rowSeparation {
	return 0.0;
}

- (id)initWithCalculator:(RSCalculator *)calculator; {
	if (!(self = [super initWithNibName:[self viewNibName] bundle:nil]))
		return nil;
	
	_calculator = [calculator retain];
	
	return self;
}

@synthesize calculator=_calculator;
@synthesize animatingOutlineView=_animatingOutlineView;

@end
