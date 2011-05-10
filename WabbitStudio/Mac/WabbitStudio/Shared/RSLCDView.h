//
//  RSLCDView.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>


#define kLCDWidth 96
#define kLCDWidescreenWidth 128
#define kLCDHeight 64
#define kLCDDisplayWidth 192
#define kLCDDisplayWidescreenWidth 256
#define kLCDDisplayHeight 128

extern NSString *const kLCDUseWirePatternKey;

@class RSCalculator,RSCalculatorSkinView;

@interface RSLCDView : NSOpenGLView {
@private	
	uint8_t _buffer[kLCDHeight][kLCDWidth][4];
	uint8_t _lcd_buffer[kLCDDisplayHeight][kLCDDisplayWidth][4];
	uint8_t _wbuffer[kLCDHeight][kLCDWidescreenWidth][4];
	uint8_t _wlcd_buffer[kLCDDisplayHeight][kLCDDisplayWidescreenWidth][4];
	uint32_t _textures[2];
	
	RSCalculator *_calculator;
	BOOL _isWidescreen;
	
	NSArray *_currentFilePaths;
}

@property (retain,nonatomic) RSCalculator *calculator;
@property (readonly,nonatomic) RSCalculatorSkinView *skinView;
@property (assign,nonatomic) BOOL isWidescreen;
@property (readonly,nonatomic) BOOL hasSkin;
@property (readonly,nonatomic) BOOL skinIsBorderless;

- (void)commonInit;

- (void)adjustSkinViewRespectingUserDefaults:(BOOL)respectUserDefaults;

- (IBAction)toggleScreenCapture:(id)sender;
- (IBAction)saveScreenCaptureAs:(id)sender;
@end
