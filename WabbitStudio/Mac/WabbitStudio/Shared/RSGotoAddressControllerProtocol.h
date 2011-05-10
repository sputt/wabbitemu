//
//  RSGotoAddressControllerProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@class RSCalculator;

@protocol RSGotoAddressController <NSObject>
@required
- (RSCalculator *)calculator;
- (NSView *)view;
- (void)scrollToAddress:(uint16_t)address;
@end
