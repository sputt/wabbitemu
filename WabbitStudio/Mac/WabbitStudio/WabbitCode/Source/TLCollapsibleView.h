//
//  TLCollapsibleView.h
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

enum {
	TLCollapsibleViewExpansionAnimation = 1 << 0,
	TLCollapsibleViewCollapseAnimation = 1 << 1
};
typedef NSUInteger TLCollapsibleViewAnimationType;

@protocol TLCollapsibleDetailView;
@class TLDisclosureBar;
@interface TLCollapsibleView : NSView <NSCoding,NSAnimationDelegate> {
@private
	TLDisclosureBar *_disclosureBar;
	NSView <TLCollapsibleDetailView> *_detailView;
	BOOL _expanded;
	NSRect _targetFrame;
	BOOL _animating;
	NSViewAnimation *_expandAnimation;
	NSViewAnimation *_collapseAnimation;
}
@property(readonly,retain) TLDisclosureBar *disclosureBar;
@property(readwrite,retain) NSView <TLCollapsibleDetailView> *detailView;
@property(readonly) BOOL expanded;
@property(readwrite,assign) BOOL animating;
@property(readwrite,assign) BOOL hasDisclosureButton;

// the following two dictionaries contain entries for the keys below.
extern NSString *TLCollapsibleViewAnimationTypeKey; // an NSNumber TLCollapsibleViewAnimationType
extern NSString *TLCollapsibleViewAnimationInfoKey; // an NSDictionary the animation info for the TLCollapsibleView
extern NSString *TLCollapsibleViewDetailViewAnimationInfoKey; // an NSDictionary the animation info for the detailView

// on calling -expandAnimationInfo and -collapseAnimationInfo, the detail view will receive a -canCollapse or canExpand message if the optional methods of the TLCollapsibleDetailView protocol are implemented. If these return NO, then -expandAnimationInfo and -collapseAnimationInfo return nil;
@property(readonly) NSDictionary *expandAnimationInfo;
@property(readonly) NSDictionary *collapseAnimationInfo;

- (id)initWithFrame:(NSRect)frame detailView:(NSView <TLCollapsibleDetailView> *)detailView expanded:(BOOL)expanded;
- (void)expand;
- (void)collapse;

// If the view is a subview of a TLCollapsibleOutlineView then the TLCollapsibleOutlineView will manage the animation and the animtion of any other subviews it has. If not then TLCollapsibleView istelf will animate.
- (IBAction)toggleExpansionState:(id)sender;
@end

@protocol TLCollapsibleDetailView
@optional
- (void)viewWillExpand;
- (void)viewDidExpand;
- (void)viewWillCollapse;
- (void)viewDidCollapse;
- (BOOL)canCollapse;
- (BOOL)canExpand; // not likely a NO, but included for completeness.
@end

@protocol TLCollapsibleViewCollection
- (void)view:(TLCollapsibleView *)collapsibleView willResizeWithFrame:(NSRect)frame;
@end

// this notification is sent only when the height of the detail view is changed such that the frame of the TLCollapsibleView needs to increase or decrease. Thus enabling the adjustment of subviews of the TLAnimatingOutlineView. Allowing the developer to alter the contents of the detail view at runtime. The userInfo dictionary contains the posting object under the @"NSObject" key.
extern NSString *TLCollapsibleViewDetailViewDidChangeFrameNotification;
