//
//  TLAnimatingOutlineView.m
//  Created by Jonathan Dann and on 20/10/2008.
//	Copyright (c) 2008, espresso served here.
//	All rights reserved.
//
//	Redistribution and use in source and binary forms, with or without modification, 
//	are permitted provided that the following conditions are met:
//
//	Redistributions of source code must retain the above copyright notice, this list 
//	of conditions and the following disclaimer.
//
//	Redistributions in binary form must reproduce the above copyright notice, this list 
//	of conditions and the following disclaimer in the documentation and/or other materials 
//	provided with the distribution.
//
//	Neither the name of the espresso served here nor the names of its contributors may be
//	used to endorse or promote products derived from this software without specific prior 
//	written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
//	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
//	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
//	OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// If you use it, acknowledgement in an About Page or other appropriate place would be nice.
// For example, "Contains code from "TLAnimatingOutlineView" by Jonathan Dann http://code.google.com/p/tlanimatingoutlineview/" will do.

#import <Quartz/Quartz.h>
#import "TLAnimatingOutlineView.h"
#import "TLCollapsibleView.h"
#import "TLDisclosureBar.h"


NSString *TLAnimatingOutlineViewItemWillExpandNotification = @"TLAnimatingOutlineViewItemWillExpandNotification";
NSString *TLAnimatingOutlineViewItemDidExpandNotification = @"TLAnimatingOutlineViewItemDidExpandNotification";
NSString *TLAnimatingOutlineViewItemWillCollapseNotification = @"TLAnimatingOutlineViewItemWillCollapseNotification";
NSString *TLAnimatingOutlineViewItemDidCollapseNotification = @"TLAnimatingOutlineViewItemDidCollapseNotification";

@interface TLAnimatingOutlineView ()
@property(readwrite,assign) BOOL animating;
@property(readwrite,copy) NSViewAnimation *insertionAnimation;
@property(readwrite,copy) NSViewAnimation *removalAnimation;
@property(readwrite,copy) NSViewAnimation *expandAnimation;
@property(readwrite,copy) NSViewAnimation *collapseAnimation;
@end

@interface TLAnimatingOutlineView (Private)
- (void)_updateDisclosureBarBorders;
- (void)_sizeToFit;
- (void)_temporarilyExpandFrame;
- (NSArray *)_viewAnimationsForSubviewsPositionedBelowSubview:(TLCollapsibleView *)animatedSubview withTargetFrame:(NSRect)targetFrame;
- (void)_animateSubviewsWithAnimationInfo:(NSDictionary *)info;
- (void)_postWillExpandNotificationWithItem:(TLCollapsibleView *)item;
- (void)_postDidExpandNotificationWithItem:(TLCollapsibleView *)item;
- (void)_postWillCollapseNotificationWithItem:(TLCollapsibleView *)item;
- (void)_postDidCollapseNotificationWithItem:(TLCollapsibleView *)item;
- (void)_removeDelegateAsObserver;
- (void)_adjustSubviewFrameOrigins;
- (void)_subviewDidChangeFrame:(NSNotification *)notification;
@end

@implementation TLAnimatingOutlineView (Private)
- (void)_updateDisclosureBarBorders;
{
	if ([[self subviews] count] <= 1 || [self.delegate rowSeparation] > 0.0f)
		return;
	
	if ([self.delegate rowSeparation] < 1.0 && [self numberOfRows] > 0)
		[[[[self subviews] objectAtIndex:0] disclosureBar] setBorderSidesMask:TLMinYEdge];
	
	for (NSUInteger index = 1 ; index < [[self subviews] count] ; index ++) {
		TLCollapsibleView *subview = [[self subviews] objectAtIndex:index];
		TLCollapsibleView *precedingView = [[self subviews] objectAtIndex:index - 1];
		[[subview disclosureBar] setBorderSidesMask:precedingView.expanded ? (TLMinYEdge|TLMaxYEdge) : ([[subview disclosureBar] isFlipped] ? TLMaxYEdge : TLMinYEdge)];
	}
}

- (void)_sizeToFit;
{
	if ([[self subviews] count] == 0)
		return;
	
	NSRect newViewFrame = [self frame];
	newViewFrame.size.height = ([self numberOfRows] > 0) ? 0.0f : [self.delegate rowSeparation];
		
	for (TLCollapsibleView *subview in [self subviews])
		newViewFrame.size.height += NSHeight([subview frame]) + [self.delegate rowSeparation];
	[self setFrame:newViewFrame];
}

- (void)_temporarilyExpandFrame;
{
	// If we're expanding/inserting/adding a view animatedly, we temporarily change our frame's height to that of the NSClip view (if we're in a scroll view) so the subviews drawings aren't clipped as they move down the screen. Our frame's size is finalised after the animation is complete. We only expand as far as the content height for two reasons: 1) drawing is limited to the NSClip view's bounds anyway 2) scrollbar's appear juddery if we expand to the full neccessary height to encompass the expanded views from a height that is less than the clip view's height.
	if ([self enclosingScrollView]) {
		NSSize contentSize = [[self enclosingScrollView] contentSize];
		NSRect tempFrame = [self frame];
		tempFrame.size.height = MAX(tempFrame.size.height,contentSize.height); // only set our frame to the content's height if the current frame is less than the content's height.
		[self setFrame:tempFrame];
	}
}

- (NSArray *)_viewAnimationsForSubviewsPositionedBelowSubview:(TLCollapsibleView *)animatedSubview withTargetFrame:(NSRect)targetFrame;
{
	NSMutableArray *animationsForOtherSubviews = [NSMutableArray array];
	NSRect newPrecedingViewFrame = targetFrame;
	for (NSUInteger index = [[self subviews] indexOfObject:animatedSubview] + 1; index < [[self subviews] count] ; index++) {
		NSView *subview = [[self subviews] objectAtIndex:index];
		NSRect newSubviewFrame = [subview frame];
		newSubviewFrame.origin.y = NSMaxY(newPrecedingViewFrame) + [self.delegate rowSeparation];
		NSDictionary *subviewAnimationInfo = [NSDictionary dictionaryWithObjectsAndKeys:subview,NSViewAnimationTargetKey,[NSValue valueWithRect:[subview frame]],NSViewAnimationStartFrameKey,[NSValue valueWithRect:newSubviewFrame],NSViewAnimationEndFrameKey,nil];
		[animationsForOtherSubviews addObject:subviewAnimationInfo];
		newPrecedingViewFrame = newSubviewFrame;
	}
	return [[animationsForOtherSubviews copy] autorelease];
}

- (void)_animateSubviewsWithAnimationInfo:(NSDictionary *)info;
{
	if (self.animating)
		return;
	
	TLCollapsibleView *animatingSubview = [[info objectForKey:TLCollapsibleViewAnimationInfoKey] objectForKey:NSViewAnimationTargetKey];
	NSRect animatingSubviewEndFrame = [[[info objectForKey:TLCollapsibleViewAnimationInfoKey] objectForKey:NSViewAnimationEndFrameKey] rectValue];
	
	NSMutableArray *allViewAnimations = [NSMutableArray arrayWithObjects:[info objectForKey:TLCollapsibleViewAnimationInfoKey],[info objectForKey:TLCollapsibleViewDetailViewAnimationInfoKey],nil];
	
	[allViewAnimations addObjectsFromArray:[self _viewAnimationsForSubviewsPositionedBelowSubview:animatingSubview withTargetFrame:animatingSubviewEndFrame]];
	
	if ([[info objectForKey:TLCollapsibleViewAnimationTypeKey] unsignedIntValue] == TLCollapsibleViewExpansionAnimation)
		[self _temporarilyExpandFrame];
	
	NSViewAnimation *animation = [[[NSViewAnimation alloc] initWithViewAnimations:allViewAnimations] autorelease];
	[animation setDuration:0.25];
	[animation setDelegate:self];
	[animation setAnimationCurve:NSAnimationEaseInOut];
	[animation startAnimation];
}

- (void)_postWillExpandNotificationWithItem:(TLCollapsibleView *)item;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:TLAnimatingOutlineViewItemWillExpandNotification object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:item,@"NSObject",nil]];	
}

- (void)_postDidExpandNotificationWithItem:(TLCollapsibleView *)item;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:TLAnimatingOutlineViewItemDidExpandNotification object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:item,@"NSObject",nil]];	
}

- (void)_postWillCollapseNotificationWithItem:(TLCollapsibleView *)item;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:TLAnimatingOutlineViewItemWillCollapseNotification object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:item,@"NSObject",nil]];	
}

- (void)_postDidCollapseNotificationWithItem:(TLCollapsibleView *)item;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:TLAnimatingOutlineViewItemDidCollapseNotification object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:item,@"NSObject",nil]];	
}

- (void)_removeDelegateAsObserver;
{
	// we explicitly remove the delegate as observer of these notifications, else we potentially remove it from observing those that the client code has set up.
	[[NSNotificationCenter defaultCenter] removeObserver:self.delegate name:TLAnimatingOutlineViewItemWillExpandNotification object:self];
	[[NSNotificationCenter defaultCenter] removeObserver:self.delegate name:TLAnimatingOutlineViewItemDidExpandNotification object:self];
	[[NSNotificationCenter defaultCenter] removeObserver:self.delegate name:TLAnimatingOutlineViewItemWillCollapseNotification object:self];
	[[NSNotificationCenter defaultCenter] removeObserver:self.delegate name:TLAnimatingOutlineViewItemDidCollapseNotification object:self];	
}

- (void)_adjustSubviewFrameOrigins;
{
	NSView *previousSubview = nil;
	NSRect viewFrame = NSZeroRect;
	for (NSView *view in [self subviews]) {
		viewFrame = [view frame];
		if (previousSubview != nil)
			viewFrame.origin.y = NSMaxY([previousSubview frame]) + [self.delegate rowSeparation];
		else // we're looking at the subview at index 0. This handles the case where subview at index 0 is removed.
			viewFrame.origin.y = 0.0f + [self.delegate rowSeparation];
		[view setFrame:viewFrame];
		previousSubview = view;
	}
}

- (void)_subviewDidChangeFrame:(NSNotification *)notification;
{
	[self _adjustSubviewFrameOrigins];
	[self _updateDisclosureBarBorders];
	[self _sizeToFit];
}

@end

@implementation TLAnimatingOutlineView
@synthesize animating = _animating;
@synthesize insertionAnimation = _insertionAnimation;
@synthesize removalAnimation = _removalAnimation;
@synthesize expandAnimation = _expandAnimation;
@synthesize collapseAnimation = _collapseAnimation;
@synthesize allowsSingleSubviewExpansion = _allowsSingleSubviewExpansion;

- (id)initWithFrame:(NSRect)frame;
{
	if (![super initWithFrame:frame])
		return nil;
    return self;
}

- (NSArray *)keysForCoding;
{
	return [NSArray arrayWithObjects:nil];
}

- (id)initWithCoder:(NSCoder *)coder;
{
	if (![super initWithCoder:coder])
		return nil;
	for (NSString *key in [self keysForCoding])
		[coder encodeObject:[self valueForKey:key] forKey:key];
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder;
{
	for (NSString *key in [self keysForCoding])
		[self setValue:[coder decodeObjectForKey:key] forKey:key];
	[super encodeWithCoder:coder];
}

- (void)dealloc;
{
	[self _removeDelegateAsObserver];
	for (NSView *subview in [self subviews])
		[[NSNotificationCenter defaultCenter] removeObserver:self name:TLCollapsibleViewDetailViewDidChangeFrameNotification object:subview];
	[_insertionAnimation release];
	[_removalAnimation release];
	[_expandAnimation release];
	[_collapseAnimation release];
	[super dealloc];
}

- (BOOL)isFlipped;
{
	return YES;
}

@dynamic delegate;
- (id<TLAnimatingOutlineViewDelegate>)delegate {
	return _delegate;
}
- (void)setDelegate:(id <TLAnimatingOutlineViewDelegate>)delegate;
{
	if (_delegate == delegate)
		return;
	[self _removeDelegateAsObserver];
	_delegate = delegate;
	
	if ([(id)_delegate respondsToSelector:@selector(outlineViewItemWillExpand:)])
		[[NSNotificationCenter defaultCenter] addObserver:_delegate selector:@selector(outlineViewItemWillExpand:) name:TLAnimatingOutlineViewItemWillExpandNotification object:self];
	if ([(id)_delegate respondsToSelector:@selector(outlineViewItemDidExpand:)])
		[[NSNotificationCenter defaultCenter] addObserver:_delegate selector:@selector(outlineViewItemWillExpand:) name:TLAnimatingOutlineViewItemDidExpandNotification object:self];
	if ([(id)_delegate respondsToSelector:@selector(outlineViewItemWillCollapse:)])
		[[NSNotificationCenter defaultCenter] addObserver:_delegate selector:@selector(outlineViewItemWillExpand:) name:TLAnimatingOutlineViewItemWillCollapseNotification object:self];
	if ([(id)_delegate respondsToSelector:@selector(outlineViewItemDidCollapse:)])
		[[NSNotificationCenter defaultCenter] addObserver:_delegate selector:@selector(outlineViewItemWillExpand:) name:TLAnimatingOutlineViewItemDidCollapseNotification object:self];
}

- (TLCollapsibleView *)addViewWithViewController:(NSViewController *)viewController image:(NSImage *)image expanded:(BOOL)expanded animate:(BOOL)animate;
{
	return [self addView:[viewController view] withImage:nil label:[viewController title] expanded:expanded animate:animate];
}

- (TLCollapsibleView *)addViewWithViewController:(NSViewController *)viewController image:(NSImage *)image expanded:(BOOL)expanded;
{
	return [self addViewWithViewController:viewController image:image expanded:expanded animate:NO];
}

- (TLCollapsibleView *)addViewWithViewController:(NSViewController *)viewController;
{
	return [self addViewWithViewController:viewController image:nil expanded:YES animate:NO];
}

- (TLCollapsibleView *)addView:(NSView *)detailView withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded animate:(BOOL)animate;
{
	NSRect collapsibleViewFrame = [self frame];
	collapsibleViewFrame.size.height = NSHeight([detailView frame]); // the initialiser of TLCollapsibleView reserves the right to increase the height of the view to include the disclosure bar frame
	if ([[self subviews] count] != 0)
		collapsibleViewFrame.origin.y = NSMaxY([[[self subviews] lastObject] frame]) + [self.delegate rowSeparation];
	else
		collapsibleViewFrame.origin.y = [self.delegate rowSeparation];
	
	TLCollapsibleView *collapsibleView = [[[TLCollapsibleView alloc] initWithFrame:collapsibleViewFrame detailView:detailView expanded:expanded] autorelease];
	[[collapsibleView disclosureBar] setLabel:label];
	[[collapsibleView disclosureBar] setLeftImage:image];
	[self addSubview:collapsibleView];	
	
	if (animate) {
		NSDictionary *fadeInAnimationInfo = [NSDictionary dictionaryWithObjectsAndKeys:collapsibleView,NSViewAnimationTargetKey,NSViewAnimationFadeInEffect,NSViewAnimationEffectKey,nil];
			
		[self _temporarilyExpandFrame];
		
		self.insertionAnimation = [[[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:fadeInAnimationInfo,nil]] autorelease];
		[self.insertionAnimation setDuration:0.25];
		[self.insertionAnimation setAnimationCurve:NSAnimationEaseInOut];
		[self.insertionAnimation setDelegate:self];
		[self.insertionAnimation startAnimation];
	} else {
		[self _adjustSubviewFrameOrigins];
		[self _updateDisclosureBarBorders];
		[self _sizeToFit];	
	}
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_subviewDidChangeFrame:) name:TLCollapsibleViewDetailViewDidChangeFrameNotification object:collapsibleView];
	return collapsibleView;
}

- (TLCollapsibleView *)addView:(NSView *)detailView withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded;
{
	return [self addView:detailView withImage:image label:label expanded:expanded animate:NO];
}

- (TLCollapsibleView *)addView:(NSView *)detailView;
{
	return [self addView:detailView withImage:nil label:nil expanded:YES];
}

- (TLCollapsibleView *)insertView:(NSView *)detailView atRow:(NSUInteger)row withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded animate:(BOOL)animate;
{
	if (!detailView)
		return nil;
	if (row >= [[self subviews] count])
		return [self addView:detailView withImage:image label:label expanded:expanded];

	NSMutableArray *subviews = [self mutableArrayValueForKey:@"subviews"];
	
	NSRect collapsibleViewFrame = [self frame];
	collapsibleViewFrame.size.height = NSHeight([detailView frame]);
	collapsibleViewFrame.origin.y = NSMinY([[[self subviews] objectAtIndex:row] frame]) + [self.delegate rowSeparation];
	TLCollapsibleView *collapsibleView = [[[TLCollapsibleView alloc] initWithFrame:collapsibleViewFrame detailView:detailView expanded:expanded] autorelease];
	[[collapsibleView disclosureBar] setLabel:label];
	[[collapsibleView disclosureBar] setLeftImage:image];
	[subviews insertObject:collapsibleView atIndex:row];
	
	if (animate) {
		NSDictionary *fadeInAnimationInfo = [NSDictionary dictionaryWithObjectsAndKeys:collapsibleView,NSViewAnimationTargetKey,NSViewAnimationFadeInEffect,NSViewAnimationEffectKey,nil];
		
		NSMutableArray *allViewAnimations = [NSMutableArray arrayWithObjects:fadeInAnimationInfo,nil];
		[allViewAnimations addObjectsFromArray:[self _viewAnimationsForSubviewsPositionedBelowSubview:collapsibleView withTargetFrame:[collapsibleView frame]]]; // this is recalculated as TLCollapsibleView reserves the right to alter the frame passed to it.
		[self _temporarilyExpandFrame];
		
		self.insertionAnimation = [[[NSViewAnimation alloc] initWithViewAnimations:allViewAnimations] autorelease];
		[self.insertionAnimation setDuration:0.25];
		[self.insertionAnimation setAnimationCurve:NSAnimationEaseInOut];
		[self.insertionAnimation setDelegate:self];
		[self.insertionAnimation startAnimation];
	} else {		
		[self _adjustSubviewFrameOrigins];
		[self _updateDisclosureBarBorders];
		[self _sizeToFit];
	}
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_subviewDidChangeFrame:) name:TLCollapsibleViewDetailViewDidChangeFrameNotification object:collapsibleView];
	return collapsibleView;
}

- (TLCollapsibleView *)insertView:(NSView *)detailView atRow:(NSUInteger)row withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded;
{
	return [self insertView:detailView atRow:row withImage:image label:label expanded:expanded animate:NO];
}

- (void)removeItem:(TLCollapsibleView *)item animate:(BOOL)animate;
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:TLCollapsibleViewDetailViewDidChangeFrameNotification object:item];
	if (animate) {
		NSDictionary *fadeOutAnimationInfo = [NSDictionary dictionaryWithObjectsAndKeys:item,NSViewAnimationTargetKey,NSViewAnimationFadeOutEffect,NSViewAnimationEffectKey,nil];
		
		NSMutableArray *allViewAnimations = [NSMutableArray arrayWithObject:fadeOutAnimationInfo];
		NSRect targetFrame = [item frame];
		targetFrame.size.height = 0.0f; // This is the only case where the target frame is not the same as that of the animating subview.
		[allViewAnimations addObjectsFromArray:[self _viewAnimationsForSubviewsPositionedBelowSubview:item withTargetFrame:targetFrame]];
		
		self.removalAnimation = [[[NSViewAnimation alloc] initWithViewAnimations:allViewAnimations] autorelease];
		[self.removalAnimation setDuration:0.4];
		[self.removalAnimation setAnimationCurve:NSAnimationEaseInOut];
		[self.removalAnimation setDelegate:self];
		[self.removalAnimation startAnimation];
	} else {
		NSMutableArray *subviews = [self mutableArrayValueForKey:@"subviews"];
		[subviews removeObjectIdenticalTo:item];
		[self _adjustSubviewFrameOrigins];
		[self _updateDisclosureBarBorders];
		[self _sizeToFit];
	}
}

- (void)removeItem:(TLCollapsibleView *)item;
{
	[self removeItem:item animate:NO];
}

- (void)removeItemAtRow:(NSUInteger)row animate:(BOOL)animate;
{
	if (row >= [[self subviews] count])
		return;
	/*
	if (row < 0)
		return;
	 */
	
	[self removeItem:[self itemAtRow:row] animate:animate];	
}

- (void)removeItemAtRow:(NSUInteger)row;
{
	[self removeItemAtRow:row animate:NO];
}

- (BOOL)isItemExpanded:(TLCollapsibleView *)item;
{
	return item.expanded;
}

- (void)expandItem:(TLCollapsibleView *)item;
{
	if (self.animating)
		return;
	
	BOOL shouldExpand = NO;	
	if (![(id)self.delegate respondsToSelector:@selector(outlineView:shouldExpandItem:)])
		shouldExpand = YES;
	else
		shouldExpand = [self.delegate outlineView:self shouldExpandItem:item];
	
	if (!shouldExpand)
		return;
	
	NSDictionary *animationInfo = [item expandAnimationInfo];
	if(!animationInfo)
		return;
	
	[self _postWillExpandNotificationWithItem:item];
	if ([[item detailView] respondsToSelector:@selector(viewWillExpand)])
		[[item detailView] viewWillExpand];
	
	[self _animateSubviewsWithAnimationInfo:animationInfo];
}

- (void)collapseItem:(TLCollapsibleView *)item;
{
	if (self.animating)
		return;
	
	BOOL shouldCollapse = NO;
	if (![(id)self.delegate respondsToSelector:@selector(outlineView:shouldCollapseItem:)])
		shouldCollapse = YES;
	else
		shouldCollapse = [self.delegate outlineView:self shouldCollapseItem:item];
	
	if (!shouldCollapse)
		return;
	
	NSDictionary *animationInfo = [item collapseAnimationInfo];
	if(!animationInfo) // the TLCollapsibleView will also ask the detail view if it can expand/collapse.
		return;

	[self _postWillCollapseNotificationWithItem:item];	
	if ([[item detailView] respondsToSelector:@selector(viewWillCollapse)])
		[[item detailView] viewWillCollapse];

	[self _animateSubviewsWithAnimationInfo:animationInfo];
}

- (NSUInteger)numberOfRows;
{
	return [[self subviews] count];
}

- (void)expandItemAtRow:(NSUInteger)row;
{
	if (row >= [[self subviews] count])
		return;
	/*
	if (row < 0)
		return;
	 */
	[self expandItem:[self itemAtRow:row]];
}

- (void)collapseItemAtRow:(NSUInteger)row;
{
	if (row >= [[self subviews] count])
		return;
	/*
	if (row < 0)
		return;
	 */
	[self collapseItem:[self itemAtRow:row]];
}

- (TLCollapsibleView *)itemAtRow:(NSUInteger)row;
{
	if (row >= [[self subviews] count])
		return nil;
	/*
	if (row < 0)
		return nil;
	 */
	return [[self subviews] objectAtIndex:row];
}

- (NSView *)detailViewAtRow:(NSUInteger)row;
{
	if (row >= [[self subviews] count])
		return nil;
	/*
	if (row < 0)
		return nil;
	 */
	return [[self itemAtRow:row] detailView];
}

- (NSUInteger)rowForItem:(TLCollapsibleView *)item;
{
	return [[self subviews] indexOfObjectIdenticalTo:item];
}

- (NSUInteger)rowForDetailView:(NSView *)detailView;
{
	return [[[self subviews] valueForKey:@"detailView"] indexOfObjectIdenticalTo:detailView];
}

@end

@implementation TLAnimatingOutlineView (NSViewAnimationDelegate)
- (BOOL)animationShouldStart:(NSAnimation *)animation;
{
	self.animating = YES;
	[[self subviews] makeObjectsPerformSelector:@selector(setAnimating:) withObject:[NSNumber numberWithBool:YES]];
	return YES;
}

- (void)animationDidEnd:(NSAnimation *)animation;
{
	self.animating = NO;
	[[self subviews] makeObjectsPerformSelector:@selector(setAnimating:) withObject:[NSNumber numberWithBool:NO]];
	
	TLCollapsibleView *animatedSubview = [[[(NSViewAnimation *)animation viewAnimations] objectAtIndex:0] objectForKey:NSViewAnimationTargetKey];
	
	if (animation == self.removalAnimation) {
		// note that this is currently the same code that happens when removing a row without animation
		NSMutableArray *subviews = [self mutableArrayValueForKey:@"subviews"];
		[subviews removeObjectIdenticalTo:animatedSubview];
	} else {
		if ([(id)self.delegate respondsToSelector:animatedSubview.expanded ? @selector(outlineViewItemDidExpand:) : @selector(outlineViewItemDidCollapse:)])
			animatedSubview.expanded ? [self _postDidExpandNotificationWithItem:animatedSubview] : [self _postDidCollapseNotificationWithItem:animatedSubview];
		if ([animatedSubview.detailView respondsToSelector:animatedSubview.expanded ? @selector(viewDidExpand) : @selector(viewDidExpand)])
			animatedSubview.expanded ? [animatedSubview.detailView viewDidExpand] : [animatedSubview.detailView viewDidCollapse];
		[[animatedSubview.disclosureBar disclosureButton] setState:animatedSubview.expanded ? NSOnState : NSOffState];
	}
	
	// cleanup
	[self _adjustSubviewFrameOrigins];
	[self _updateDisclosureBarBorders];
	[self _sizeToFit];
}

@end
