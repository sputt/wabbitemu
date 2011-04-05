//
//  WCLineHighlighter.m
//  WabbitStudio
//
//  Created by William Towe on 3/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCLineHighlighter.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "NSObject+WCExtensions.h"
#import "WCTextStorage.h"

@implementation WCLineHighlighter

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	// stop observing the preferences values
	[self cleanupUserDefaultsObserving];
	// unregister for the selection change notification
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_textView = nil;
    [super dealloc];
}

- (id)initWithTextView:(NSTextView *)textView; {
	if (!(self = [super init]))
		return nil;
	
	_textView = textView;
	
	// register to receive selection change notifications from our text view
	[self setupNotificationObserving];
	[self setupUserDefaultsObserving];
	
	return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	
	// compare against the context value to see which value we are observing
	if ([(NSString *)context isEqualToString:kWCPreferencesCurrentLineHighlightKey]) {
		// check the current value and highlight the line or disable it
		[_textView setNeedsDisplay:YES];
	}
	else if ([(NSString *)context isEqualToString:kWCPreferencesCurrentLineHighlightColorKey]) {
		// if drawing is enabled draw with the new color
		[_textView setNeedsDisplay:YES];
	}
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesCurrentLineHighlightKey,kWCPreferencesCurrentLineHighlightColorKey, nil];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_textViewDidChangeSelection:)),kNSObjectSelectorKey,NSTextViewDidChangeSelectionNotification,kNSObjectNotificationNameKey,_textView,kNSObjectNotificationObjectKey, nil], nil];
}

- (void)_textViewDidChangeSelection:(NSNotification *)note {
	// return early if we aren't supposed to highlight the line
	if (![[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesCurrentLineHighlightKey])
		return;
	
	[_textView setNeedsDisplayInRect:[_textView visibleRect]];
}
@end
