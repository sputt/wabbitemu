//
//  WELCDView.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>


#define kLCDWidth 96
#define kLCDWidescreenWidth 128
#define kLCDHeight 64

extern NSString *const kLCDUseWirePatternKey;

@class RSCalculator;

@interface WELCDView : NSOpenGLView {
@private	
	GLubyte _buffer[kLCDHeight][kLCDWidth][4];
	GLubyte _lcd_buffer[kLCDHeight][kLCDWidth][4];
	GLubyte _wbuffer[kLCDHeight][kLCDWidescreenWidth][4];
	GLubyte _wlcd_buffer[kLCDHeight][kLCDWidescreenWidth][4];
	GLuint _textures[2];
	
	RSCalculator *_calculator;
	BOOL _isWidescreen;
	
	NSArray *_currentFilePaths;
}

@property (retain,nonatomic) RSCalculator *calculator;
@property (assign,nonatomic) BOOL isWidescreen;

- (void)commonInit;
@end
