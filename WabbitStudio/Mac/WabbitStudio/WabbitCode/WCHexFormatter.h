//
//  WCHexFormatter.h
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSFormatter.h>


static inline u_int8_t HexValueForCharacter(unichar character) {
	switch (character) {
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			return 0;
	}
}

static inline u_int8_t ValueForCharacter(unichar character) {
	switch (character) {
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		default:
			return 0;
	}
}

static inline u_int8_t BinaryValueForCharacter(unichar character) {
	switch (character) {
		case '0':
			return 0;
		case '1':
			return 1;
		default:
			return 0;
	}
}

@interface NSString (WCHexFormatter_Extensions)
- (NSString *)stringByRemovingInvalidHexDigits;
@end

@interface WCHexFormatter : NSFormatter {
@private
    
}

@end
