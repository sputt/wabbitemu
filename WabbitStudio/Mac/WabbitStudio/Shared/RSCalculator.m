//
//  RSCalculator.m
//  WabbitStudio
//
//  Created by William Towe on 5/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSCalculator.h"
#import "WCHexFormatter.h"
#include "disassemble.h"
#include "alu.h"
#include "ti_stdint.h"


static void RSCalculatorBreakpointCallback(LPCALC theCalc,void *info) {
	RSCalculator *calculator = (RSCalculator *)info;
	
	[[calculator owner] performSelector:[calculator breakpointSelector]];
}

NSString *const kRSCalculatorModelDidChangeNotification = @"kRSCalculatorModelDidChangeNotification";

@implementation RSCalculator
+ (NSSet *)keyPathsForValuesAffectingValueForKey:(NSString *)key {
	// most things only depend on the program counter changing
	if ([key isEqualToString:@"stackPointer"] ||
		[key isEqualToString:@"registerAFPrime"] ||
		[key isEqualToString:@"registerBC"] ||
		[key isEqualToString:@"registerBCPrime"] ||
		[key isEqualToString:@"registerDE"] ||
		[key isEqualToString:@"registerDEPrime"] ||
		[key isEqualToString:@"registerHL"] ||
		[key isEqualToString:@"registerHLPrime"] ||
		[key isEqualToString:@"registerIX"] ||
		[key isEqualToString:@"registerIY"] ||
		[key isEqualToString:@"CPUHalt"] ||
		[key isEqualToString:@"CPUBus"] ||
		[key isEqualToString:@"CPUFrequency"] ||
		[key isEqualToString:@"memoryMapBank0RamOrFlash"] ||
		[key isEqualToString:@"memoryMapBank1RamOrFlash"] ||
		[key isEqualToString:@"memoryMapBank2RamOrFlash"] ||
		[key isEqualToString:@"memoryMapBank3RamOrFlash"] ||
		[key isEqualToString:@"memoryMapBank0Readonly"] ||
		[key isEqualToString:@"memoryMapBank1Readonly"] ||
		[key isEqualToString:@"memoryMapBank2Readonly"] ||
		[key isEqualToString:@"memoryMapBank3Readonly"] ||
		[key isEqualToString:@"memoryMapBank0Page"] ||
		[key isEqualToString:@"memoryMapBank1Page"] ||
		[key isEqualToString:@"memoryMapBank2Page"] ||
		[key isEqualToString:@"memoryMapBank3Page"] ||
		[key isEqualToString:@"interruptsIFF1"] ||
		[key isEqualToString:@"interruptsIFF2"] ||
		[key isEqualToString:@"interruptsNextTimer1"] ||
		[key isEqualToString:@"interruptsNextTimer2"] ||
		[key isEqualToString:@"interruptsTimer1Duration"] ||
		[key isEqualToString:@"interruptsTimer2Duration"] ||
		[key isEqualToString:@"displayActive"] ||
		[key isEqualToString:@"displayContrast"] ||
		[key isEqualToString:@"displayX"] ||
		[key isEqualToString:@"displayY"] ||
		[key isEqualToString:@"displayZ"] ||
		[key isEqualToString:@"displayCursorMode"])
		return [NSSet setWithObjects:@"programCounter", nil];
	// register AF in particular depends on the PC and on all the flags
	else if ([key isEqualToString:@"registerAF"])
		return [NSSet setWithObjects:@"programCounter",@"zFlag",@"cFlag",@"sFlag",@"pvFlag",@"hcFlag",@"nFlag", nil];
	// likewise all the flags depend on both the PC and register AF
	else if ([key isEqualToString:@"zFlag"] ||
			 [key isEqualToString:@"cFlag"] ||
			 [key isEqualToString:@"sFlag"] ||
			 [key isEqualToString:@"pvFlag"] ||
			 [key isEqualToString:@"hcFlag"] ||
			 [key isEqualToString:@"nFlag"])
		return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
	return [super keyPathsForValuesAffectingValueForKey:key];
}

- (void)dealloc {
	_owner = nil;
	_breakpointSelector = NULL;
	calc_slot_free(_calc);
    [super dealloc];
}

+ (RSCalculator *)calculatorWithOwner:(id <RSCalculatorOwner>)owner breakpointSelector:(SEL)breakpointSelector; {
	return [[[[self class] alloc] initWithOwner:owner breakpointSelector:breakpointSelector] autorelease];
}
- (id)initWithOwner:(id <RSCalculatorOwner>)owner breakpointSelector:(SEL)breakpointSelector; {
	if (!(self = [super init]))
		return nil;
	
	_calc = calc_slot_new();
	
	if (_calc == NULL)
		return nil;
	
	_owner = owner;
	_breakpointSelector = breakpointSelector;
	
	_calc->breakpoint_callback = &RSCalculatorBreakpointCallback;
	_calc->breakpoint_owner = (void *)self;
	
	return self;
}

- (void)step; {
	[self willChangeValueForKey:@"programCounter"];
	
	CPU_step(&[self calc]->cpu);
	
	[self didChangeValueForKey:@"programCounter"];
}

- (void)stepOver; {
	lpDebuggerCalc = [self calc];
	
	CPU_t *cpu = &[self calc]->cpu;
	const int usable_commands[] = { DA_BJUMP, DA_BJUMP_N, DA_BCALL_N, DA_BCALL,
		DA_BLI, DA_CALL_X, DA_CALL_CC_X, DA_HALT, DA_RST_X};
	int i;
	double time = tc_elapsed(cpu->timer_c);
	Z80_info_t zinflocal;
	
	disassemble(cpu->mem_c, cpu->pc, 1, &zinflocal);
	
	[self willChangeValueForKey:@"programCounter"];
	
	if (cpu->halt) {
		if (cpu->iff1) {
			while ((tc_elapsed(cpu->timer_c) - time) < 15.0 && cpu->halt == TRUE )
				CPU_step(cpu);
		} else {
			cpu->halt = FALSE;
		}
	} else if (zinflocal.index == DA_CALL_X || zinflocal.index == DA_CALL_CC_X) {
		uint16_t old_stack = cpu->sp;
		CPU_step(cpu);
		if (cpu->sp != old_stack) {
			[self stepOut];
		}
	} else {
		for (i = 0; i < NumElm(usable_commands); i++) {
			if (zinflocal.index == usable_commands[i]) {
				while ((tc_elapsed(cpu->timer_c) - time) < 15.0 && cpu->pc != (zinflocal.addr + zinflocal.size))
					CPU_step(cpu);
				return;
			}
		}
		CPU_step(cpu);
	}
	
	[self didChangeValueForKey:@"programCounter"];
}

- (void)stepOut; {
	lpDebuggerCalc = [self calc];
	
	CPU_t *cpu = &[self calc]->cpu;
	double time = tc_elapsed(cpu->timer_c);
	uint16_t old_sp = cpu->sp;
	
	while ((tc_elapsed(cpu->timer_c) - time) < 15.0) {
		waddr_t old_pc = addr_to_waddr(cpu->mem_c, cpu->pc);
		CPU_step(cpu);
		
		if (cpu->sp > old_sp) {
			Z80_info_t zinflocal;
			disassemble(cpu->mem_c, old_pc.addr, 1, &zinflocal);
			
			if (zinflocal.index == DA_RET 		||
				zinflocal.index == DA_RET_CC 	||
				zinflocal.index == DA_RETI		||
				zinflocal.index == DA_RETN) {
				
				return;
			}
			
		}
	}
}

- (BOOL)loadRomOrSavestate:(NSString *)romOrSavestatePath error:(NSError **)outError; {
	[self setIsRunning:NO];
	[self setIsBusy:YES];
	
	RSCalculatorModel oldModel = [self model];
	BOOL loaded = rom_load([self calc], [romOrSavestatePath fileSystemRepresentation]);
	
	if (!loaded) {
		[self setIsBusy:NO];
		
		return NO;
	}
	
	calc_turn_on([self calc]);
	
	[self setIsRunning:YES];
	[self setIsBusy:NO];
	
	if (oldModel != [self model])
		[[NSNotificationCenter defaultCenter] postNotificationName:kRSCalculatorModelDidChangeNotification object:self];
	
	return YES;
}

- (void)setBreakpointOfType:(RSBreakpointType)breakpointType atAddress:(uint16_t)address onPage:(uint8_t)page; {
	set_break(&[self calc]->mem_c, (breakpointType == RSBreakpointTypeRam)?TRUE:FALSE, page, address);
}

- (void)simulateKeyPress:(uint16_t)keyCode {
	[self simulateKeyPress:keyCode lastKeyPressInSeries:NO];
}

- (void)simulateKeyPress:(uint16_t)keyCode lastKeyPressInSeries:(BOOL)lastKeyPressInSeries; {
	keypad_key_press(&[self calc]->cpu, keyCode);
	calc_run_seconds([self calc], 0.25);
	keypad_key_release(&[self calc]->cpu, keyCode);
	if (!lastKeyPressInSeries)
		calc_run_seconds([self calc], 0.25);
}

@synthesize calc=_calc;
@dynamic isActive;
- (BOOL)isActive {
	return ([self calc] != NULL && [self calc]->active);
}
- (void)setIsActive:(BOOL)isActive {
	if ([self calc] != NULL)
		[self calc]->active = isActive;
}
@dynamic isRunning;
- (BOOL)isRunning {
	return ([self calc] != NULL && [self calc]->running);
}
- (void)setIsRunning:(BOOL)isRunning {
	if ([self calc] != NULL)
		[self calc]->active = isRunning;
}
@synthesize isBusy=_isBusy;
@synthesize owner=_owner;
@synthesize breakpointSelector=_breakpointSelector;
@dynamic model;
- (RSCalculatorModel)model {
	if ([self calc] == NULL)
		return NSNotFound;
	return (RSCalculatorModel)[self calc]->model;
}
#pragma mark Skin Images
@dynamic skinImage;
- (NSImage *)skinImage {
	switch ([self model]) {
		case RSCalculatorModelTI73:
			return [NSImage imageNamed:@"ti-73"];
		case RSCalculatorModelTI82:
			return [NSImage imageNamed:@"ti-82"];
		case RSCalculatorModelTI83:
			return [NSImage imageNamed:@"ti-83"];
		case RSCalculatorModelTI83P:
			return [NSImage imageNamed:@"ti-83+"];
		case RSCalculatorModelTI83PSE:
			return [NSImage imageNamed:@"ti-83+se"];
		case RSCalculatorModelTI84P:
			return [NSImage imageNamed:@"ti-84+"];
		case RSCalculatorModelTI84PSE:
			return [NSImage imageNamed:@"ti-84+se"];
		case RSCalculatorModelTI85:
			return [NSImage imageNamed:@"ti-85"];
		case RSCalculatorModelTI86:
			return [NSImage imageNamed:@"ti-86"];
		default:
			return nil;
	}
}
@dynamic keymapImage;
- (NSImage *)keymapImage {
	switch ([self model]) {
		case RSCalculatorModelTI82:
			return [NSImage imageNamed:@"ti-82keymap"];
		case RSCalculatorModelTI83:
			return [NSImage imageNamed:@"ti-83keymap"];
		case RSCalculatorModelTI73:
		case RSCalculatorModelTI83P:
		case RSCalculatorModelTI83PSE:
			return [NSImage imageNamed:@"ti-83+keymap"];
		case RSCalculatorModelTI84P:
		case RSCalculatorModelTI84PSE:
			return [NSImage imageNamed:@"ti-84+sekeymap"];
		case RSCalculatorModelTI85:
			return [NSImage imageNamed:@"ti-85keymap"];
		case RSCalculatorModelTI86:
			return [NSImage imageNamed:@"ti-86keymap"];
		default:
			return nil;
	}
}
#pragma mark Program Counter
@dynamic programCounter;
- (uint16_t)programCounter {
	if ([self calc] != NULL)
		return [self calc]->cpu.pc;
	return 0;
}
- (void)setProgramCounter:(uint16_t)programCounter {
	if ([self calc] != NULL)
		[self calc]->cpu.pc = programCounter;
}
#pragma mark Stack Pointer
@dynamic stackPointer;
- (uint16_t)stackPointer {
	if ([self calc] != NULL)
		return [self calc]->cpu.sp;
	return 0;
}
- (void)setStackPointer:(uint16_t)stackPointer {
	if ([self calc] != NULL)
		[self calc]->cpu.sp = stackPointer;
}
#pragma mark Registers
@dynamic registerAF;
- (uint16_t)registerAF {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.af;
}
- (void)setRegisterAF:(uint16_t)registerAF {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.af = registerAF;
}

@dynamic registerAFPrime;
- (uint16_t)registerAFPrime {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.afp;
}
- (void)setRegisterAFPrime:(uint16_t)registerAFPrime {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.afp = registerAFPrime;
}

@dynamic registerBC;
- (uint16_t)registerBC {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.bc;
}
- (void)setRegisterBC:(uint16_t)registerBC {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.bc = registerBC;
}

@dynamic registerBCPrime;
- (uint16_t)registerBCPrime {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.bcp;
}
- (void)setRegisterBCPrime:(uint16_t)registerBCPrime {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.bcp = registerBCPrime;
}

@dynamic registerDE;
- (uint16_t)registerDE {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.de;
}
- (void)setRegisterDE:(uint16_t)registerDE {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.de = registerDE;
}

@dynamic registerDEPrime;
- (uint16_t)registerDEPrime {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.dep;
}
- (void)setRegisterDEPrime:(uint16_t)registerDEPrime {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.dep = registerDEPrime;
}

@dynamic registerHL;
- (uint16_t)registerHL {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.hl;
}
- (void)setRegisterHL:(uint16_t)registerHL {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.hl = registerHL;
}

@dynamic registerHLPrime;
- (uint16_t)registerHLPrime {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.hlp;
}
- (void)setRegisterHLPrime:(uint16_t)registerHLPrime {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.hlp = registerHLPrime;
}

@dynamic registerIX;
- (uint16_t)registerIX {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.ix;
}
- (void)setRegisterIX:(uint16_t)registerIX {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.ix = registerIX;
}

@dynamic registerIY;
- (uint16_t)registerIY {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.iy;
}
- (void)setRegisterIY:(uint16_t)registerIY {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.iy = registerIY;
}
#pragma mark Flags
- (BOOL)zFlag {
	return ([self calc] != NULL && ([self calc]->cpu.f & ZERO_MASK) != 0);
}

- (void)setZFlag:(BOOL)value {	
	[self calc]->cpu.f ^= ZERO_MASK;
}

- (BOOL)cFlag {
	return ([self calc] != NULL && ([self calc]->cpu.f & CARRY_MASK) != 0);
}

- (void)setCFlag:(BOOL)value {
	[self calc]->cpu.f ^= CARRY_MASK;
}

- (BOOL)sFlag {
	return ([self calc] != NULL && ([self calc]->cpu.f & SIGN_MASK) != 0);
}

- (void)setSFlag:(BOOL)value {
	[self calc]->cpu.f ^= SIGN_MASK;
}

- (BOOL)pvFlag {
	return ([self calc] != NULL && ([self calc]->cpu.f & PV_MASK) != 0);
}

- (void)setPvFlag:(BOOL)value {
	[self calc]->cpu.f ^= PV_MASK;
}

- (BOOL)hcFlag {
	return ([self calc] != NULL && ([self calc]->cpu.f & HC_MASK) != 0);
}

- (void)setHcFlag:(BOOL)value {
	[self calc]->cpu.f ^= HC_MASK;
}

- (BOOL)nFlag {
	return ([self calc] != NULL && ([self calc]->cpu.f & N_MASK) != 0);
}

- (void)setNFlag:(BOOL)value {
	[self calc]->cpu.f ^= N_MASK;
}
#pragma mark CPU
@dynamic CPUHalt;
- (BOOL)CPUHalt {
	return ([self calc] != NULL && [self calc]->cpu.halt);
}
- (void)setCPUHalt:(BOOL)CPUHalt {	
	[self calc]->cpu.halt = CPUHalt;
}
@dynamic CPUBus;
- (uint8_t)CPUBus {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.bus;
}
- (void)setCPUBus:(uint8_t)CPUBus {	
	[self calc]->cpu.bus = CPUBus;
}

@dynamic CPUFrequency;
- (uint32_t)CPUFrequency {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->timer_c.freq;
}
- (void)setCPUFrequency:(uint32_t)CPUFrequency {
	[self calc]->timer_c.freq = CPUFrequency;
}
#pragma mark Memory Map
@dynamic memoryMapBank0RamOrFlash;
- (BOOL)memoryMapBank0RamOrFlash {
	return ([self calc] != NULL && [self calc]->mem_c.banks[0].ram);
}
- (void)setMemoryMapBank0RamOrFlash:(BOOL)memoryMapBank0RamOrFlash {
	[self calc]->mem_c.banks[0].ram = memoryMapBank0RamOrFlash;
}

@dynamic memoryMapBank0Readonly;
- (BOOL)memoryMapBank0Readonly {
	return ([self calc] != NULL && [self calc]->mem_c.banks[0].read_only);
}
- (void)setMemoryMapBank0Readonly:(BOOL)memoryMapBank0Readonly {
	[self calc]->mem_c.banks[0].read_only = memoryMapBank0Readonly;
}

@dynamic memoryMapBank0Page;
- (uint8_t)memoryMapBank0Page {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->mem_c.banks[0].page;
}
- (void)setMemoryMapBank0Page:(uint8_t)memoryMapBank0Page {
	[self calc]->mem_c.banks[0].page = memoryMapBank0Page;
}

@dynamic memoryMapBank1RamOrFlash;
- (BOOL)memoryMapBank1RamOrFlash {
	return ([self calc] != NULL && [self calc]->mem_c.banks[1].ram);
}
- (void)setMemoryMapBank1RamOrFlash:(BOOL)memoryMapBank1RamOrFlash {
	[self calc]->mem_c.banks[1].ram = memoryMapBank1RamOrFlash;
}

@dynamic memoryMapBank1Readonly;
- (BOOL)memoryMapBank1Readonly {
	return ([self calc] != NULL && [self calc]->mem_c.banks[1].read_only);
}
- (void)setMemoryMapBank1Readonly:(BOOL)memoryMapBank1Readonly {
	[self calc]->mem_c.banks[1].read_only = memoryMapBank1Readonly;
}

@dynamic memoryMapBank1Page;
- (uint8_t)memoryMapBank1Page {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->mem_c.banks[1].page;
}
- (void)setMemoryMapBank1Page:(uint8_t)memoryMapBank1Page {
	[self calc]->mem_c.banks[1].page = memoryMapBank1Page;
}

@dynamic memoryMapBank2RamOrFlash;
- (BOOL)memoryMapBank2RamOrFlash {
	return ([self calc] != NULL && [self calc]->mem_c.banks[2].ram);
}
- (void)setMemoryMapBank2RamOrFlash:(BOOL)memoryMapBank2RamOrFlash {
	[self calc]->mem_c.banks[2].ram = memoryMapBank2RamOrFlash;
}

@dynamic memoryMapBank2Readonly;
- (BOOL)memoryMapBank2Readonly {
	return ([self calc] != NULL && [self calc]->mem_c.banks[2].read_only);
}
- (void)setMemoryMapBank2Readonly:(BOOL)memoryMapBank2Readonly {
	[self calc]->mem_c.banks[2].read_only = memoryMapBank2Readonly;
}

@dynamic memoryMapBank2Page;
- (uint8_t)memoryMapBank2Page {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->mem_c.banks[2].page;
}
- (void)setMemoryMapBank2Page:(uint8_t)memoryMapBank2Page {
	[self calc]->mem_c.banks[2].page = memoryMapBank2Page;
}

@dynamic memoryMapBank3RamOrFlash;
- (BOOL)memoryMapBank3RamOrFlash {
	return ([self calc] != NULL && [self calc]->mem_c.banks[3].ram);
}
- (void)setMemoryMapBank3RamOrFlash:(BOOL)memoryMapBank3RamOrFlash {
	[self calc]->mem_c.banks[3].ram = memoryMapBank3RamOrFlash;
}

@dynamic memoryMapBank3Readonly;
- (BOOL)memoryMapBank3Readonly {
	return ([self calc] != NULL && [self calc]->mem_c.banks[3].read_only);
}
- (void)setMemoryMapBank3Readonly:(BOOL)memoryMapBank3Readonly {
	[self calc]->mem_c.banks[3].read_only = memoryMapBank3Readonly;
}

@dynamic memoryMapBank3Page;
- (uint8_t)memoryMapBank3Page {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->mem_c.banks[3].page;
}
- (void)setMemoryMapBank3Page:(uint8_t)memoryMapBank3Page {
	[self calc]->mem_c.banks[3].page = memoryMapBank3Page;
}
#pragma mark Interrupts
@dynamic interruptsIFF1;
- (BOOL)interruptsIFF1 {
	return ([self calc] != NULL && [self calc]->cpu.iff1);
}
- (void)setInterruptsIFF1:(BOOL)interruptsIFF1 {
	[self calc]->cpu.iff1 = interruptsIFF1;
}

@dynamic interruptsNextTimer1;
- (double)interruptsNextTimer1 {
	if ([self calc] == NULL)
		return 0;
	return (([self calc]->timer_c.elapsed - [self calc]->cpu.pio.stdint->lastchk1) * 1000);
}

@dynamic interruptsTimer1Duration;
- (double)interruptsTimer1Duration {
	if ([self calc] == NULL)
		return 0;
	return ([self calc]->cpu.pio.stdint->timermax1 * 1000);
}

@dynamic interruptsIFF2;
- (BOOL)interruptsIFF2 {
	return ([self calc] != NULL && [self calc]->cpu.iff2);
}
- (void)setInterruptsIFF2:(BOOL)interruptsIFF2 {	
	[self calc]->cpu.iff2 = interruptsIFF2;
}

@dynamic interruptsNextTimer2;
- (double)interruptsNextTimer2 {
	if ([self calc] == NULL)
		return 0;
	return (([self calc]->timer_c.elapsed - [self calc]->cpu.pio.stdint->lastchk2) * 1000);
}

@dynamic interruptsTimer2Duration;
- (double)interruptsTimer2Duration {
	if ([self calc] == NULL)
		return 0;
	return ([self calc]->cpu.pio.stdint->timermax2 * 1000);
}
#pragma mark Display
@dynamic displayActive;
- (BOOL)displayActive {
	return ([self calc] != NULL && [self calc]->cpu.pio.lcd->active);
}
- (void)setDisplayActive:(BOOL)displayActive {
	[self calc]->cpu.pio.lcd->active = displayActive;
}

@dynamic displayContrast;
- (uint32_t)displayContrast {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.pio.lcd->contrast;
}
- (void)setDisplayContrast:(uint32_t)displayContrast {
	[self calc]->cpu.pio.lcd->contrast = displayContrast;
}

@dynamic displayX;
- (int32_t)displayX {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.pio.lcd->x;
}
- (void)setDisplayX:(int32_t)displayX {
	[self calc]->cpu.pio.lcd->x = displayX;
}

@dynamic displayY;
- (int32_t)displayY {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.pio.lcd->y;
}
- (void)setDisplayY:(int32_t)displayY {
	[self calc]->cpu.pio.lcd->y = displayY;
}

@dynamic displayZ;
- (int32_t)displayZ {
	if ([self calc] == NULL)
		return 0;
	return [self calc]->cpu.pio.lcd->z;
}
- (void)setDisplayZ:(int32_t)displayZ {
	[self calc]->cpu.pio.lcd->z = displayZ;
}

@dynamic displayCursorMode;
- (LCD_CURSOR_MODE)displayCursorMode {
	if ([self calc] == NULL)
		return MODE_NONE;
	return [self calc]->cpu.pio.lcd->cursor_mode;
}
- (void)setDisplayCursorMode:(LCD_CURSOR_MODE)displayCursorMode {
	[self calc]->cpu.pio.lcd->cursor_mode = displayCursorMode;
}
@end
