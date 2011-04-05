//
//  TLAnimatingOutlineView.h
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

#import <Cocoa/Cocoa.h>

@protocol TLAnimatingOutlineViewDelegate;
@class TLCollapsibleView;
@interface TLAnimatingOutlineView : NSView <NSCoding,NSAnimationDelegate> {
@private
	id <TLAnimatingOutlineViewDelegate> _delegate;
	BOOL _animating;
	NSViewAnimation *_insertionAnimation;
	NSViewAnimation *_removalAnimation;
	NSViewAnimation *_expandAnimation;
	NSViewAnimation *_collapseAnimation;
	BOOL _allowsSingleSubviewExpansion;
}
@property(readwrite,assign) id <TLAnimatingOutlineViewDelegate> delegate;
@property(readonly) BOOL animating;

// WARNING: not yet implemented
@property(readwrite,assign) BOOL allowsSingleSubviewExpansion; // Forces other expanded subviews to collapse when one is opened. The behaviour of this when set to YES while multiple views are expanded is to come into effect when a subview is expanded.

// all methods taking an NSViewController as a parameter set the label of the row to the title of the view controller
- (TLCollapsibleView *)addViewWithViewController:(NSViewController *)viewController image:(NSImage *)image expanded:(BOOL)expanded animate:(BOOL)animate;
- (TLCollapsibleView *)addViewWithViewController:(NSViewController *)viewController image:(NSImage *)image expanded:(BOOL)expanded;
- (TLCollapsibleView *)addViewWithViewController:(NSViewController *)viewController;

- (TLCollapsibleView *)addView:(NSView *)detailView withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded animate:(BOOL)animate;
- (TLCollapsibleView *)addView:(NSView *)detailView withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded;
- (TLCollapsibleView *)addView:(NSView *)detailView;

// when inserting, if the specified row is < 0 a new TLCollapsibleView will be created and placed at row 0, if > numberOfRows it is simply added to the rows.
- (TLCollapsibleView *)insertView:(NSView *)detailView atRow:(NSUInteger)row withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded animate:(BOOL)animate;
- (TLCollapsibleView *)insertView:(NSView *)detailView atRow:(NSUInteger)row withImage:(NSImage *)image label:(NSString *)label expanded:(BOOL)expanded;

- (void)removeItem:(TLCollapsibleView *)item animate:(BOOL)animate;
- (void)removeItem:(TLCollapsibleView *)item;
- (void)removeItemAtRow:(NSUInteger)row animate:(BOOL)animate;
- (void)removeItemAtRow:(NSUInteger)row;

- (void)expandItem:(TLCollapsibleView *)item;
- (void)expandItemAtRow:(NSUInteger)row;
- (void)collapseItem:(TLCollapsibleView *)item;
- (void)collapseItemAtRow:(NSUInteger)row;

- (NSUInteger)numberOfRows;

// simply returns the item at the specified index of the reciever's subviews array. Returns nil for out of bounds access.
- (TLCollapsibleView *)itemAtRow:(NSUInteger)row;
- (NSView *)detailViewAtRow:(NSUInteger)row;

- (NSUInteger)rowForItem:(TLCollapsibleView *)item;
- (NSUInteger)rowForDetailView:(NSView *)detailView;

- (BOOL)isItemExpanded:(TLCollapsibleView *)item;

@end

@protocol TLAnimatingOutlineViewDelegate
@required
- (CGFloat)rowSeparation;
@optional
- (BOOL)outlineView:(TLAnimatingOutlineView *)outlineView shouldExpandItem:(TLCollapsibleView *)item;
- (BOOL)outlineView:(TLAnimatingOutlineView *)outlineView shouldCollapseItem:(TLCollapsibleView *)item;

// The delegate object is automatically registered for the correct notifications if these methods are implemented.
- (void)outlineViewItemWillExpand:(NSNotification *)notification;
- (void)outlineViewItemDidExpand:(NSNotification *)notification;
- (void)outlineViewItemWillCollapse:(NSNotification *)notification;
- (void)outlineViewItemDidCollapse:(NSNotification *)notification;
@end

// These notifications contain a userInfo dictionary with a reference to the relevant TLCollapsibleView under the @"NSObject" key.
extern NSString *TLAnimatingOutlineViewItemWillExpandNotification;
extern NSString *TLAnimatingOutlineViewItemDidExpandNotification;
extern NSString *TLAnimatingOutlineViewItemWillCollapseNotification;
extern NSString *TLAnimatingOutlineViewItemDidCollapseNotification;
