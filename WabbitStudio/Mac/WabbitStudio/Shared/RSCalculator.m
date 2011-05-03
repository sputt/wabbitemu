//
//  RSCalculator.m
//  WabbitStudio
//
//  Created by William Towe on 5/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSCalculator.h"
#include "disassemble.h"
#include "alu.h"


static void RSCalculatorBreakpointCallback(LPCALC theCalc,void *info) {
	RSCalculator *calculator = (RSCalculator *)info;
	
	[[calculator owner] performSelector:[calculator breakpointSelector]];
}

@implementation RSCalculator

+ (NSSet *)keyPathsForValuesAffectingStackPointer {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterAF {
    return [NSSet setWithObjects:@"programCounter",@"zFlag",@"cFlag",@"sFlag",@"pvFlag",@"hcFlag",@"nFlag", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterAFPrime {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterBC {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterBCPrime {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterDE {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterDEPrime {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterHL {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterHLPrime {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterIX {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingRegisterIY {
    return [NSSet setWithObjects:@"programCounter", nil];
}
+ (NSSet *)keyPathsForValuesAffectingZFlag {
    return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
}
+ (NSSet *)keyPathsForValuesAffectingCFlag {
    return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
}
+ (NSSet *)keyPathsForValuesAffectingSFlag {
    return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
}
+ (NSSet *)keyPathsForValuesAffectingPvFlag {
    return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
}
+ (NSSet *)keyPathsForValuesAffectingHcFlag {
    return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
}
+ (NSSet *)keyPathsForValuesAffectingNFlag {
    return [NSSet setWithObjects:@"programCounter",@"registerAF", nil];
}

- (void)dealloc {
	_owner = nil;
	_breakpointSelector = NULL;
	calc_slot_free(_calc);
    [super dealloc];
}

+ (RSCalculator *)calculatorWithOwner:(NSDocument <RSCalculatorOwner> *)owner breakpointSelector:(SEL)breakpointSelector; {
	return [[[[self class] alloc] initWithOwner:owner breakpointSelector:breakpointSelector] autorelease];
}
- (id)initWithOwner:(NSDocument <RSCalculatorOwner> *)owner breakpointSelector:(SEL)breakpointSelector; {
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
	
	BOOL loaded = rom_load([self calc], [romOrSavestatePath fileSystemRepresentation]);
	
	if (!loaded) {
		[self setIsBusy:NO];
		
		return NO;
	}
	
	calc_turn_on([self calc]);
	
	[self setIsBusy:NO];
	
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
	if ([self calc] == NULL)
		return NO;
	return (([self calc]->cpu.f & ZERO_MASK) != 0);
}

- (void)setZFlag:(BOOL)value {	
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.f ^= ZERO_MASK;
}

- (BOOL)cFlag {
	if ([self calc] == NULL)
		return NO;
	return (([self calc]->cpu.f & CARRY_MASK) != 0);
}

- (void)setCFlag:(BOOL)value {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.f ^= CARRY_MASK;
}

- (BOOL)sFlag {
	if ([self calc] == NULL)
		return NO;
	return (([self calc]->cpu.f & SIGN_MASK) != 0);
}

- (void)setSFlag:(BOOL)value {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.f ^= SIGN_MASK;
}

- (BOOL)pvFlag {
	if ([self calc] == NULL)
		return NO;
	return (([self calc]->cpu.f & PV_MASK) != 0);
}

- (void)setPvFlag:(BOOL)value {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.f ^= PV_MASK;
}

- (BOOL)hcFlag {
	if ([self calc] == NULL)
		return NO;
	return ([self calc]->cpu.f & HC_MASK);
}

- (void)setHcFlag:(BOOL)value {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.f ^= HC_MASK;
}

- (BOOL)nFlag {
	if ([self calc] == NULL)
		return NO;
	return ([self calc]->cpu.f & N_MASK);
}

- (void)setNFlag:(BOOL)value {
	if ([self calc] == NULL)
		return;
	
	[self calc]->cpu.f ^= N_MASK;
}
@end
