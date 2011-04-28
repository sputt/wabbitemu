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


static const NSUInteger kLCDWidth = 192;
static const NSUInteger kLCDWidescreenWidth = 256;
static const NSUInteger kLCDHeight = 128;

@interface WELCDView ()
@property (copy,nonatomic) NSArray *currentFilePaths;

- (void)_privateInit;
- (void)_resizeForWidescreen;
@end

@implementation WELCDView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self _privateInit];
	
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
	
    NSRect frame = [self frame];
	GLint width = (GLint)NSWidth(frame), height = (GLint)NSHeight(frame);
	u_char *lcd = LCD_image([self calc]->cpu.pio.lcd);
	u_int row, col;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWEPreferencesDisplayUseLCDWirePatternKey])
		glEnable(GL_BLEND);
	
	if ([self isWidescreen]) {
		for (row=0; row<kLCDHeight; row++) {
			for (col=0; col<kLCDWidescreenWidth; col++) {
				u_char val = 255-lcd[(row/2)*128+(col/2)];
				
				_wbuffer[row][col][2] = (0x9E*val)/255;
				_wbuffer[row][col][1] = (0xAB*val)/255;
				_wbuffer[row][col][0] = (0x88*val)/255;
			}
		}
	}
	else {
		for (row=0; row<kLCDHeight; row++) {
			for (col=0; col<kLCDWidth; col++) {
				u_char val = 255-lcd[(row/2)*128+(col/2)];
				
				_buffer[row][col][2] = (0x9E*val)/255;
				_buffer[row][col][1] = (0xAB*val)/255;
				_buffer[row][col][0] = (0x88*val)/255;
			}
		}
	}
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _textures[0]);
	
	if ([self isWidescreen])
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _wbuffer);
	else
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _buffer);
	
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(width, 0); glVertex2i(width, 0);
	glTexCoord2i(width, height); glVertex2i(width, height);
	glTexCoord2i(0, height); glVertex2i(0, height);
	glEnd();
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWEPreferencesDisplayUseLCDWirePatternKey]) {		
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _textures[1]);
		
		if ([self isWidescreen])
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _wlcd_buffer);
		else
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _lcd_buffer);
		
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(0, 0);
		glTexCoord2i(width, 0); glVertex2i(width, 0);
		glTexCoord2i(width, height); glVertex2i(width, height);
		glTexCoord2i(0, height); glVertex2i(0, height);
		glEnd();
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glDisable(GL_BLEND);
	}
	
	glFinish();
}

- (void)reshape {
	NSRect frame = [self frame];
	CGFloat width = NSWidth(frame), height = NSHeight(frame);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1.0, 0.0);
	glViewport(0, 0, width, height);
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
	NSArray *filePaths = [[sender draggingPasteboard] propertyListForType:NSFilenamesPboardType];
	NSMutableArray *validFilePaths = [NSMutableArray arrayWithCapacity:[filePaths count]];
	
	for (NSString *path in filePaths) {
		NSString *UTI = [[NSWorkspace sharedWorkspace] typeOfFile:path error:NULL];
		
		if ([UTI isEqualToString:kWECalculatorProgramUTI] ||
			[UTI isEqualToString:kWECalculatorApplicationUTI])
			[validFilePaths addObject:path];
	}
	[self setCurrentFilePaths:validFilePaths];
	return ([validFilePaths count] > 0)?NSDragOperationCopy:NSDragOperationNone;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
	[WETransferSheetController transferFiles:[self currentFilePaths] toCalculator:(WECalculator *)[[[self window] windowController] document]];
	return YES;
}

@synthesize currentFilePaths=_currentFilePaths;
@synthesize calc=_calc;
@dynamic isWidescreen;
- (BOOL)isWidescreen {
	return _isWidescreen;
}
- (void)setIsWidescreen:(BOOL)isWidescreen {
	if (_isWidescreen == isWidescreen)
		return;
	
	_isWidescreen = isWidescreen;
	
	[self _resizeForWidescreen];
}
@dynamic calculator;
- (WECalculator *)calculator {
	return (WECalculator *)[[[self window] windowController] document];
}

- (void)_privateInit {

	[self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType,nil]];
	
	[[self openGLContext] makeCurrentContext];
	
	u_int16_t row, col;
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidth; col++) {
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_buffer[row][col][3] = 255;
		}
	}
	
	for (row=0; row < kLCDHeight; row++) {
		for (col=0; col < kLCDWidescreenWidth; col++) {
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_wbuffer[row][col][3] = 255;
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
	
	for (row=0; row < kLCDHeight; row++) {
		for (col=0; col < kLCDWidescreenWidth; col++) {
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
	
	NSRect frame = [self frame];
	CGFloat width = NSWidth(frame), height = NSHeight(frame);
	
	glClearColor(128.0/255.0, 142.0/255.0, 107.0/255.0, 1.0);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glGenTextures(2, _textures);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE,  GL_TRUE);
	
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}

- (void)_resizeForWidescreen; {
	NSRect frame = [[self window] frame];
	CGFloat delta = kLCDWidescreenWidth - kLCDWidth;
	if ([self isWidescreen])
		frame.size.width += delta;
	else
		frame.size.width -= delta;
	
	NSUInteger mask = [self autoresizingMask];
	[self setAutoresizingMask:NSViewWidthSizable];
	[[self window] setFrame:frame display:YES animate:YES];
	[self setAutoresizingMask:mask];
}
@end