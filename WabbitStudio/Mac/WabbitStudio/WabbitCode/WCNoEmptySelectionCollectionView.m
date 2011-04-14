//
//  WCNoEmptySelectionCollectionView.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCNoEmptySelectionCollectionView.h"


@implementation WCNoEmptySelectionCollectionView
// prevent empty selections
- (void)setSelectionIndexes:(NSIndexSet *)indexes {
	if ([indexes count] == 0)
		return;
	[super setSelectionIndexes:indexes];
}

@end
