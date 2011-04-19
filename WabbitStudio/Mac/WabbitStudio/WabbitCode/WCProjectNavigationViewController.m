//
//  WCProjectNavigationViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavigationViewController.h"
#import "NSArray+WCExtensions.h"

#import <BWToolkitFramework/BWAnchoredButtonBar.h>

@implementation WCProjectNavigationViewController

- (void)dealloc {
	_project = nil;
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
}

@synthesize project=_project;
@synthesize splitterHandleImageView=_splitterHandleImageView;
// subclasses must override this to return the correct view nib name
@dynamic viewNibName;
@dynamic selectedObject;
- (id)selectedObject {
	return [[self selectedObjects] firstObject];
}
- (void)setSelectedObject:(id)selectedObject {
	[self setSelectedObjects:[NSArray arrayWithObject:selectedObject]];
}
@dynamic selectedNode;
- (NSTreeNode *)selectedNode {
	return [[self selectedNodes] firstObject];
}
// subclasses must define selectedNodes and selectedObjects for their given context
@dynamic selectedNodes;
@dynamic selectedObjects;
@synthesize firstResponder=_firstResponder;

+ (id)projectNavigationViewControllerWithProject:(WCProject *)project; {
	return [[[[self class] alloc] initWithProject:project] autorelease];
}
- (id)initWithProject:(WCProject *)project; {
	if (!(self = [super initWithNibName:[self viewNibName] bundle:nil]))
		return nil;
	
	_project = project;
	
	return self;
}
@end
