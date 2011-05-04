//
//  RSCalculatorSkinView.m
//  WabbitStudio
//
//  Created by William Towe on 5/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSCalculatorSkinView.h"
#import "RSCalculator.h"


NSString *const kRSCalculatorSkinViewUseSkinsKey = @"appearanceUseSkins";
NSString *const kRSCalculatorSkinViewUseBorderlessSkinsKey = @"appearanceUseBorderlessSkins";

@implementation RSCalculatorSkinView

- (void)dealloc {
	[_calculator release];
    [super dealloc];
}

- (BOOL)isFlipped {
	return YES;
}

- (BOOL)acceptsFirstResponder {
	return NO;
}

- (void)mouseDown:(NSEvent *)event {
	if ([self calculator] == nil || ![[self calculator] isRunning])
		return;
	
	NSImage *keymap = [[self calculator] keymapImage];
	NSBitmapImageRep *bitmap = (NSBitmapImageRep *)[keymap bestRepresentationForRect:NSZeroRect context:nil hints:nil];
	
#ifdef DEBUG
	NSAssert(bitmap != nil, @"bitmap for keymap image was nil!");
#endif
	
	NSPoint point = [self convertPointFromBase:[event locationInWindow]];
	NSUInteger rgba[4];
	
	[bitmap getPixel:rgba atX:point.x y:point.y];
	
	uint8_t group, bit;
	keypad_t *kp = [[self calculator] calc]->cpu.pio.keypad;
	
	for(group=0;group<7;group++) {
		for(bit=0;bit<8;bit++) {
			kp->keys[group][bit] &=(~KEY_MOUSEPRESS);
		}
	}
	
	[[self calculator] calc]->cpu.pio.keypad->on_pressed &= ~KEY_MOUSEPRESS;
	
	if (rgba[0] == 0xFF) {
#ifdef DEBUG
		NSLog(@"missed clicking a button on mouse down");
#endif
		_shouldMoveWindowOnMouseDragged = YES;
		goto finalize_buttons;
	}
	
	if ((rgba[1]>>4) == 0x05 && (rgba[2]>>4) == 0x00) {
		[[self calculator] calc]->cpu.pio.keypad->on_pressed |= KEY_MOUSEPRESS;
	}
	else {
		kp->keys[rgba[1] >> 4][rgba[2] >> 4] |= KEY_MOUSEPRESS;
		if ((kp->keys[rgba[1] >> 4][rgba[2] >> 4] & KEY_STATEDOWN) == 0) {
			//DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &pt, DBS_DOWN | DBS_PRESS);
			kp->keys[rgba[1] >> 4][rgba[2] >> 4] |= KEY_STATEDOWN;
			//SendMessage(hwnd, WM_SIZE, 0, 0);
		}
	}
	
finalize_buttons:
	for(group=0;group<7;group++) {
		for(bit=0;bit<8;bit++) {
			if ((kp->keys[group][bit] & KEY_STATEDOWN) &&
				((kp->keys[group][bit] & KEY_MOUSEPRESS) == 0) &&
				((kp->keys[group][bit] & KEY_KEYBOARDPRESS) == 0)) {
				//DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &ButtonCenter[bit+(group<<3)], DBS_UP | DBS_PRESS);
				kp->keys[group][bit] &= (~KEY_STATEDOWN);
				//SendMessage(hwnd, WM_SIZE, 0, 0);
			}
		}
	}
}

- (void)mouseDragged:(NSEvent *)theEvent {
	if (!_shouldMoveWindowOnMouseDragged)
		return;
	
	NSPoint windowOrigin = [[self window] frame].origin;
	
    [[self window] setFrameOrigin:NSMakePoint(windowOrigin.x + [theEvent deltaX], windowOrigin.y - [theEvent deltaY])];
}

- (void)mouseUp:(NSEvent *)event {
	_shouldMoveWindowOnMouseDragged = NO;
	
	if ([self calculator] == nil || ![[self calculator] isRunning])
		return;
	
	NSImage *keymap = [[self calculator] keymapImage];
	NSBitmapImageRep *bitmap = (NSBitmapImageRep *)[keymap bestRepresentationForRect:NSZeroRect context:nil hints:nil];
	
#ifdef DEBUG
	NSAssert(bitmap != nil, @"bitmap for keymap image was nil!");
#endif
	
	uint8_t group, bit;
	keypad_t *kp = [[self calculator] calc]->cpu.pio.keypad;
	
	for(group=0;group<7;group++) {
		for(bit=0;bit<8;bit++) {
			kp->keys[group][bit] &=(~KEY_MOUSEPRESS);
		}
	}
	
	[[self calculator] calc]->cpu.pio.keypad->on_pressed &= ~KEY_MOUSEPRESS;
	
	for(group=0;group<7;group++) {
		for(bit=0;bit<8;bit++) {
			if ((kp->keys[group][bit] & KEY_STATEDOWN) &&
				((kp->keys[group][bit] & KEY_MOUSEPRESS) == 0) &&
				((kp->keys[group][bit] & KEY_KEYBOARDPRESS) == 0)) {
				//DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &ButtonCenter[bit+(group<<3)], DBS_UP | DBS_PRESS);
				kp->keys[group][bit] &= (~KEY_STATEDOWN);
				//SendMessage(hwnd, WM_SIZE, 0, 0);
			}
		}
	}
}

- (id)initWithCalculator:(RSCalculator *)calculator frame:(NSRect)frame; {
	if (!(self = [super initWithFrame:frame]))
		return nil;
	
	_calculator = [calculator retain];
	
	return self;
}

@synthesize calculator=_calculator;

@end
