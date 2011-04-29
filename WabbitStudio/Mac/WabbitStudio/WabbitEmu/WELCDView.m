//
//  WELCDView.m
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WELCDView.h"
#import "WECalculator.h"
#import "WEPreferencesController.h"
#import "WETransferSheetController.h"


@interface WELCDView ()
@property (copy,nonatomic) NSArray *currentFilePaths;

- (void)_privateInit;
- (void)_resizeForWidescreen;
@end

@implementation WELCDView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self commonInit];
	
	return self;
}

- (void)dealloc {
	[_currentFilePaths release];
	_calc = NULL;
	glDeleteTextures(2, _textures);
    [super dealloc];
}

- (BOOL)isOpaque {
	return YES;
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (void)keyDown:(NSEvent *)event {
	keypad_key_press(&_calc->cpu, (unsigned int)[event keyCode]);
}

- (void)keyUp:(NSEvent *)event {
	keypad_key_release(&_calc->cpu, (unsigned int)[event keyCode]);
}

- (void)drawRect:(NSRect)dirtyRect
{	
	if (_calc == NULL || !_calc->active || !_calc->running)
		return;
	
	u_int8_t *lcd = LCD_image([self calc]->cpu.pio.lcd);
	u_int16_t row, col;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWEPreferencesDisplayUseLCDWirePatternKey])
		glEnable(GL_BLEND);
	 
	if ([self isWidescreen]) {
		for (row=0; row<kLCDHeight; row++) {
			for (col=0; col<kLCDWidescreenWidth; col++) {
				u_int8_t val = 255-lcd[row*128+col];
				
				_wbuffer[row][col][2] = (0x9E*val)/255;
				_wbuffer[row][col][1] = (0xAB*val)/255;
				_wbuffer[row][col][0] = (0x88*val)/255;
				
			}
		}
	}
	else {
		for (row=0; row<kLCDHeight; row++) {
			for (col=0; col<kLCDWidth; col++) {
				u_char val = 255-lcd[(row)*128+(col)];
				
				_buffer[row][col][2] = (0x9E*val)/255;
				_buffer[row][col][1] = (0xAB*val)/255;
				_buffer[row][col][0] = (0x88*val)/255;
				
			}
		}
	}
	
	CGFloat width = ([self isWidescreen])?kLCDWidescreenWidth:kLCDWidth, height = kLCDHeight;
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, _textures[0]);
	glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStoref(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
	
	if ([self isWidescreen])
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _wbuffer);
	else
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _buffer);
		
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(0.0f, height );
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(width, height );
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(width, 0.0f );
	glVertex2f(1.0f, 1.0f);
	glEnd();
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWEPreferencesDisplayUseLCDWirePatternKey]) {		
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _textures[1]);
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glPixelStoref(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
		
		if ([self isWidescreen])
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _wlcd_buffer);
		else
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _lcd_buffer);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(0.0f, height );
		glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(width, height );
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(width, 0.0f );
		glVertex2f(1.0f, 1.0f);
		glEnd();
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glDisable(GL_BLEND);
	}
	
	glFinish();
}

- (void)reshape {
	NSRect sceneBounds;
	
	[ [ self openGLContext ] update ];
	sceneBounds = [ self bounds ];
	// Reset current viewport
	glViewport( 0, 0, sceneBounds.size.width, sceneBounds.size.height );
}

- (void)prepareOpenGL {
	glClearColor(128.0/255.0, 142.0/255.0, 107.0/255.0, 1.0);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glGenTextures(2, _textures);
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
	NSArray *filePaths = [[sender draggingPasteboard] propertyListForType:NSFilenamesPboardType];
	
	[self setCurrentFilePaths:[WETransferSheetController validateFilePaths:filePaths]];
	return ([[self currentFilePaths] count] > 0)?NSDragOperationCopy:NSDragOperationNone;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
	[WETransferSheetController transferFiles:[self currentFilePaths] toCalculator:(WECalculator *)[[[self window] windowController] document]];
	return YES;
}

- (LPCALC)calc {
	return _calc;
}
- (void)setCalc:(LPCALC)calc {
	if (_calc == calc)
		return;
	
	_calc = calc;
}

- (BOOL)isWidescreen {
	return _isWidescreen;
}
- (void)setIsWidescreen:(BOOL)isWidescreen {
	if (_isWidescreen == isWidescreen)
		return;
	
	_isWidescreen = isWidescreen;
	
	[[self calculator] resetDisplaySize:nil];
}

- (WECalculator *)calculator {
	return (WECalculator *)[[[self window] windowController] document];
}

@synthesize currentFilePaths=_currentFilePaths;


- (void)commonInit; {
	[self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
	[self _privateInit];
}

- (void)_privateInit {
	u_int16_t row, col;
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidth; col++) {
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_buffer[row][col][3] = 255;
		}
	}
	
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidth; col++) {
			if (col%2 == 0 && row%2 == 0) {
				_lcd_buffer[row][col][2] = 158;
				_lcd_buffer[row][col][1] = 171;
				_lcd_buffer[row][col][0] = 136;
			}
			else if (col%2 == 1 && row%2 == 1) {
				_lcd_buffer[row][col][2] = 126;
				_lcd_buffer[row][col][1] = 137;
				_lcd_buffer[row][col][0] = 109;
			}
			else {
				_lcd_buffer[row][col][2] = 142;
				_lcd_buffer[row][col][1] = 154;
				_lcd_buffer[row][col][0] = 122;
			}
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_lcd_buffer[row][col][3] = 108;
		}
	}
	
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidescreenWidth; col++) {
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_wbuffer[row][col][3] = 255;
		}
	}
	
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidescreenWidth; col++) {
			if (col%2 == 0 && row%2 == 0) {
				_wlcd_buffer[row][col][2] = 158;
				_wlcd_buffer[row][col][1] = 171;
				_wlcd_buffer[row][col][0] = 136;
			}
			else if (col%2 == 1 && row%2 == 1) {
				_wlcd_buffer[row][col][2] = 126;
				_wlcd_buffer[row][col][1] = 137;
				_wlcd_buffer[row][col][0] = 109;
			}
			else {
				_wlcd_buffer[row][col][2] = 142;
				_wlcd_buffer[row][col][1] = 154;
				_wlcd_buffer[row][col][0] = 122;
			}
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_wlcd_buffer[row][col][3] = 108;
		}
	}
}
@end
