#include "stdafx.h"

#include <iostream>
#include "memory.h"
#include "CPU.h"
#include "state.h"

#define PN FLAG_SIGN
#define PV FLAG_OVERFLOW
#define PB FLAG_BREAKPOINT
#define PD FLAG_DECIMAL
#define PI FLAG_INTERRUPT
#define PZ FLAG_ZERO
#define PC FLAG_CARRY

unsigned char hex2bcd(unsigned char x)
{
	unsigned char y;
	y = (x / 10) << 4;
	y = y | (x % 10);
	return (y);
}

/*	These functions require the complete and pre-calculated address to function properly. 
	If there is no parameter on the function, it's either implied or accumulator.
	Assume that PC value is correct coming in.

	The exception is when branching because we don't know if we were successful until now
	so we need to add +1 to PC on a successful branch and another +1 if the branch
	goes to a different page. */

void CPU::op_adc(WideAddress address) {
	/*
	Logic:
	  t = A + M + P.C
	  P.V = (A.7!=t.7) ? 1:0
	  P.N = A.7
	  P.Z = (t==0) ? 1:0
	  IF (P.D)
		t = bcd(A) + bcd(M) + P.C
		P.C = (t>99) ? 1:0
	  ELSE
		P.C = (t>255) ? 1:0
	  A = t & 0xFF      
	*/

	if (accumulator == 0x7f) {
		int x = 0;
	}

	uint16_t t = accumulator + fetch_memory_byte(address, false) + FLAG_CARRY;
	int8_t t8 = t & 0xff;
	PV = (~(accumulator ^ fetch_memory_byte(address, false)) & (accumulator ^ t8) & 0x80) == 0x80;
	PZ = (t8 == 0);
	PN = (t8 & 0x80) == 0x80;
	if (PD) {
		t = hex2bcd(accumulator) + hex2bcd(fetch_memory_byte(address, false)) + PC;
		PC = (t > 99);
	}
	else {
		PC = (t > 255);
	}
	accumulator = (t & 0xFF);

};

void CPU::op_adc(uint8_t immediate) {
	/*
	Logic:
		t = A + M + P.C
		P.V = (A.7 != t.7) ? 1:0
		P.N = A.7
		P.Z = (t==0) ? 1:0
		IF (P.D)
		t = bcd(A) + bcd(M) + P.C
		P.C = (t>99) ? 1:0
		ELSE
		P.C = (t>255) ? 1:0
		A = t & 0xFF
	*/

	if (accumulator == 0x7f) {
		int x = 0;
	}

	uint16_t t = accumulator + immediate + FLAG_CARRY;
	int8_t t8 = t & 0xff;
	/*/
	if ((accumulator & 0x80) != (t & 0x80)) {
		PV = true;
	}
	else {
		PV = false;
	}
	*/
	PV = (~(accumulator ^ immediate) & (accumulator ^ t) & 0x80) == 0x80;
	PN = (t8 & 0x80) == 0x80;
	PZ = (t8 == 0);
	//TODO: FLAG_DECIMAL
	if (PD) {
		t = hex2bcd(accumulator) + hex2bcd(immediate) + PC;
		PC = (t > 99);
	}
	else {
		PC = (t > 255);
	}

	accumulator = (t & 0xFF);
};

void CPU::op_and(WideAddress address) {
	/*
	Logic:
		A = A & M
		P.N = A.7
		P.Z = (A==0) ? 1:0 */
	accumulator = (accumulator & fetch_memory_byte(address, false));
	FLAG_SIGN = (accumulator & 0x80) == 0x80;
	FLAG_ZERO = (accumulator == 0);
}

void CPU::op_and(uint8_t immediate) {
	/*
	Logic:
	A = A & M
	P.N = A.7
	P.Z = (A==0) ? 1:0 */
	accumulator = (accumulator & immediate);
	FLAG_SIGN = (accumulator & 0x80) == 0x80;
	FLAG_ZERO = (accumulator == 0);
}

void CPU::op_asl(WideAddress address) { //OK
	/*
	Logic:
		P.C = B.7
		B = (B << 1) & $FE
		P.N = B.7
		P.Z = (B==0) ? 1:0
	*/
	FLAG_CARRY = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	write_memory_byte(address, (fetch_memory_byte(address, false) << 1) & 0xFE);
	FLAG_SIGN = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	FLAG_ZERO = (fetch_memory_byte(address, false) == 0);
}

void CPU::op_asl() {
	/*
	Logic:
	  P.C = B.7
	  B = (B << 1) & $FE
	  P.N = B.7
	  P.Z = (B==0) ? 1:0 
	*/
	FLAG_CARRY = (accumulator & 0x80) == 0x80;
	accumulator = (accumulator << 1) & 0xFE;
	FLAG_SIGN = (accumulator & 0x80) == 0x80;
	FLAG_ZERO = (accumulator == 0);
}

void CPU::op_bcc(int8_t relative) {
	if (!FLAG_CARRY) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_bcs(int8_t relative) {
	if (FLAG_CARRY) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_beq(int8_t relative) {
	if (FLAG_ZERO) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_bit(WideAddress address) {
	/*
	Logic:
	  t = A & M
	  P.N = M.7
	  P.V = M.6
	  P.Z = (t==0) ? 1:0 
	*/
	uint8_t operand = fetch_memory_byte(address, false);
	uint8_t t = accumulator & operand;
	PN = (operand & 0x80) == 0x80;
	PV = (operand & 0x40) == 0x40;
	PZ = (t == 0);
}

void CPU::op_bmi(int8_t relative) {
	if (PN) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_bne(int8_t relative) {
	if (!PZ) {
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
		if (relative == (int8_t)0xFE) {
			std::exit(4);
		}
	}
}

void CPU::op_bpl(int8_t relative) {
	if (!PN) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_brk() {
	uint8_t psw = getProgramStatusWord();
	setProgramStatusWord(psw |= 0x10); //enable the B flag
	psw |= 0x10; //we push the B flag as 1
	WideAddress pc = uintAddressToWideAddress(program_counter);
	stack_push(pc.high);
	stack_push(pc.low);
	stack_push(psw);
	program_counter = WideAddress{ fetch_memory_byte(0xFFFF, false), fetch_memory_byte(0xFFFE, false) };
}

void CPU::op_bvc(int8_t relative) {
	if (!PV) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_bvs(int8_t relative) {
	if (PV) {
		//+1 cycle for branching, +2 if branching to a different page
		if ((program_counter & 0x00FF) + relative > 0x100) {
			EmulatorState::Instance()->processor.cycles++;
		}
		EmulatorState::Instance()->processor.cycles++;
		program_counter += relative;
	}
}

void CPU::op_clc() {
	PC = false;
}

void CPU::op_cld() {
	PD = false;
}

void CPU::op_cli() {
	PI = false;
}

void CPU::op_clv() {
	PV = false;
}

void CPU::op_cmp(uint8_t immediate) { //immediate
	/*
	Logic:
		t = A - M
		P.N = t.7
		P.C = (A>=M) ? 1:0
		P.Z = (t==0) ? 1:0
	*/

	uint8_t t = accumulator - immediate;
	PN = (t & 0x80) == 0x80;
	PC = (accumulator >= immediate);
	PZ = (t == 0);

}

void CPU::op_cmp(WideAddress address) {
	/*
	Logic:
	t = A - M
	P.N = t.7
	P.C = (A>=M) ? 1:0
	P.Z = (t==0) ? 1:0
	*/

	uint8_t t = accumulator - fetch_memory_byte(address, false);
	PN = (t & 0x80) == 0x80;
	PC = (accumulator >= fetch_memory_byte(address, false));
	PZ = (t == 0);
}

void CPU::op_cpx(uint8_t immediate) {
	uint8_t t = index_x - immediate;
	PN = (t & 0x80) == 0x80;
	PC = (index_x >= immediate);
	PZ = (t == 0);
}

void CPU::op_cpx(WideAddress address) {
	uint8_t t = index_x - fetch_memory_byte(address, false);
	PN = (t & 0x80) == 0x80;
	PC = (index_x >= fetch_memory_byte(address, false));
	PZ = (t == 0);
}

void CPU::op_cpy(uint8_t immediate) {
	uint8_t t = index_y - immediate;
	PN = (t & 0x80) == 0x80;
	PC = (index_y >= immediate);
	PZ = (t == 0);
}

void CPU::op_cpy(WideAddress address) {
	uint8_t t = index_y - fetch_memory_byte(address, false);
	PN = (t & 0x80) == 0x80;
	PC = (index_y >= fetch_memory_byte(address, false));
	PZ = (t == 0);
}

void CPU::op_dec(WideAddress address) {
	/*
	Logic:
		M = (M - 1) & $FF
		P.N = M.7
		P.Z = (M==0) ? 1:0 
	*/

	write_memory_byte(address, (fetch_memory_byte(address, false) - 1) & 0xFF);
	PN = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	PZ = (fetch_memory_byte(address, false) == 0);

}

void CPU::op_dex() {
	index_x--;
	PZ = (index_x == 0);
	PN = (index_x & 0x80) == 0x80;
}

void CPU::op_dey() {
	index_y--;
	PZ = (index_y == 0);
	PN = (index_y & 0x80) == 0x80;
}

void CPU::op_eor(uint8_t immediate) {
	accumulator = accumulator ^ immediate;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_eor(WideAddress address) {
	accumulator = accumulator ^ fetch_memory_byte(address, false);
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_inc(WideAddress address) {
	/*
	Logic:
	M = (M + 1) & $FF
	P.N = M.7
	P.Z = (M==0) ? 1:0
	*/

	write_memory_byte(address, (fetch_memory_byte(address, false) + 1) & 0xFF);
	PN = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	PZ = (fetch_memory_byte(address, false) == 0);
}

void CPU::op_inx() {
	index_x++;
	PZ = (index_x == 0);
	PN = (index_x & 0x80) == 0x80;
}

void CPU::op_iny() {
	index_y++;
	PZ = (index_y == 0);
	PN = (index_y & 0x80) == 0x80;
}

void CPU::op_jmp(WideAddress address) {
	program_counter = address;
}

void CPU::op_jsr(WideAddress address) {
	/*
	Logic:
	t = PC - 1
	bPoke(SP, t.h)
	SP = SP - 1
	bPoke(SP, t.l)
	SP = SP - 1
	PC = address
	*/

	WideAddress t = uintAddressToWideAddress(program_counter - 1);
	stack_push(t.high);
	stack_push(t.low);
	program_counter = address;
}

void CPU::op_lda(uint8_t immediate) {
	accumulator = immediate;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_lda(WideAddress address) {
	accumulator = fetch_memory_byte(address, false);
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_ldx(uint8_t immediate) {
	index_x = immediate;
	PN = (index_x & 0x80) == 0x80;
	PZ = (index_x == 0);
}

void CPU::op_ldx(WideAddress address) {
	index_x = fetch_memory_byte(address, false);
	PN = (index_x & 0x80) == 0x80;
	PZ = (index_x == 0);
}

void CPU::op_ldy(uint8_t immediate) {
	index_y = immediate;
	PN = (index_y & 0x80) == 0x80;
	PZ = (index_y == 0);
}

void CPU::op_ldy(WideAddress address) {
	index_y = fetch_memory_byte(address, false);
	PN = (index_y & 0x80) == 0x80;
	PZ = (index_y == 0);
}

void CPU::op_lsr() { //Accumulator
	/*
	Logic:
		P.N = 0
		P.C = B.0
		B = (B >> 1) & $7F
		P.Z = (B==0) ? 1:0
	*/
	PN = false;
	PC = (accumulator & 0x01) == 0x01;
	accumulator = (accumulator >> 1) & 0x7F;
	PZ = (accumulator == 0);
}

void CPU::op_lsr(WideAddress address) { //OK
	/*
	Logic:
		P.N = 0
		P.C = B.0
		B = (B >> 1) & $7F
		P.Z = (B==0) ? 1:0
	*/
	PN = false;
	PC = (fetch_memory_byte(address, false) & 0x01) == 0x01;
	write_memory_byte(address, ((fetch_memory_byte(address, false) >> 1) & 0x7F));
	PZ = (fetch_memory_byte(address, false) == 0);
}

void CPU::op_nop() {
	//NOP
}

void CPU::op_ora(uint8_t immediate) {
	accumulator = accumulator | immediate;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_ora(WideAddress address) {
	accumulator = accumulator | fetch_memory_byte(address, false);
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_pha() {
	stack_push(accumulator);
}

void CPU::op_php() {
	stack_push(getProgramStatusWord() | 0x10);
}

void CPU::op_pla() {
	accumulator = stack_pop();
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_plp() {
	setProgramStatusWord(stack_pop());
}

void CPU::op_rol() { //Tested and working
	/*
	Logic:
	  t = B.7
	  B = (B << 1) & $FE
	  B = B | P.C
	  P.C = t
	  P.Z = (B==0) ? 1:0
	  P.N = B.7
	*/
	bool t = (accumulator & 0x80) == 0x80;
	accumulator = (accumulator << 1) & 0xFE;
	if (PC) {
		accumulator = accumulator | 0x01;
	}
	PC = t;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_rol(WideAddress address) { //Tested and working
	/*
	Logic:
		t = B.7
		B = (B << 1) & $FE
		B = B | P.C
		P.C = t
		P.Z = (B==0) ? 1:0
		P.N = B.7
	*/
	bool t = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	write_memory_byte(address, (fetch_memory_byte(address, false) << 1) & 0xFE);
	if (PC) {
		write_memory_byte(address, fetch_memory_byte(address, false) | 0x01);
	}
	PC = t;
	PN = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	PZ = (fetch_memory_byte(address, false) == 0);
}

void CPU::op_ror() { //Tested and working
	/*
	Logic:
	  t = B.0
	  B = (B >> 1) & $7F
	  B = B | ((P.C) ? $80:$00)
	  P.C = t
	  P.Z = (B==0) ? 1:0
	  P.N = B.7
	*/
	bool t = (accumulator & 0x01) == 0x01;
	accumulator = (accumulator >> 1) & 0x7F;
	if (PC) {
		accumulator |= 0x80;
	}
	PC = t;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_ror(WideAddress address) { //Tested and working
	bool t = (fetch_memory_byte(address, false) & 0x01) == 0x01;
	write_memory_byte(address, (fetch_memory_byte(address, false) >> 1) & 0x7F);
	if (PC) {
		write_memory_byte(address, (fetch_memory_byte(address, false) | 0x80));
	}
	PC = t;
	PN = (fetch_memory_byte(address, false) & 0x80) == 0x80;
	PZ = (fetch_memory_byte(address, false) == 0);
}

void CPU::op_rti() {
	setProgramStatusWord(stack_pop());
	uint8_t lo = stack_pop();
	uint8_t hi = stack_pop();
	program_counter = WideAddress{ hi, lo };
}

void CPU::op_rts() {
	uint8_t lo = stack_pop();
	uint8_t hi = stack_pop();
	WideAddress addr = WideAddress{ hi, lo };
	program_counter = addr.add(1, false);
}

void CPU::op_sbc(uint8_t immediate) {
	/*
	Logic:
	  IF (P.D)
		t = bcd(A) - bcd(M) - !P.C
		P.V = (t>99 OR t<0) ? 1:0
	  ELSE
		t = A - M - !P.C
		P.V = (t>127 OR t<-128) ? 1:0
	  P.C = (t>=0) ? 1:0
	  P.N = t.7
	  P.Z = (t==0) ? 1:0
	  A = t & 0xFF 
  */
	//TODO: decimal

	op_adc(~immediate);

	/*
	int8_t t;

	if (PD) {
		t = hex2bcd(accumulator) - hex2bcd(immediate) - !PC;
		PV = (t > 99 || t < 0);
	}
	else {
		t = accumulator - immediate - !PC;
		PV = (t > 127 || t < -128);
	}
	PC = (t >= 0);
	PN = (t & 0x80) == 0x80;
	PZ = (t == 0);
	accumulator = (t & 0xFF);
	*/
}

void CPU::op_sbc(WideAddress address) {
	op_adc(~fetch_memory_byte(address, false));

	/*
	int8_t t;

	if (PD) {
		t = hex2bcd(accumulator) - hex2bcd(fetch_memory_byte(address, false)) - !PC;
		PV = (t > 99 || t < 0);
	}
	else {
		t = accumulator - fetch_memory_byte(address, false) - !PC;
		PV = (t > 127 || t < -128);
	}

	PC = (t >= 0);
	PN = (t & 0x80) == 0x80;
	PZ = (t == 0);
	accumulator = (t & 0xFF);
	*/
}

void CPU::op_sec() {
	PC = true;
}

void CPU::op_sed() {
	PD = true;
}

void CPU::op_sei() {
	PI = true;
}

void CPU::op_sta(WideAddress address) {
	write_memory_byte(address, accumulator);
}

void CPU::op_stx(WideAddress address) {
	write_memory_byte(address, index_x);
}

void CPU::op_sty(WideAddress address) {
	write_memory_byte(address, index_y);
}

void CPU::op_tax() {
	index_x = accumulator;
	PN = (index_x & 0x80) == 0x80;
	PZ = (index_x == 0);
}

void CPU::op_tay() {
	index_y = accumulator;
	PN = (index_y & 0x80) == 0x80;
	PZ = (index_y == 0);
}

void CPU::op_tsx() {
	index_x = stack_pointer;
	PN = (index_x & 0x80) == 0x80;
	PZ = (index_x == 0);
}

void CPU::op_txa() {
	accumulator = index_x;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}

void CPU::op_txs() {
	stack_pointer = index_x;
}

void CPU::op_tya() {
	accumulator = index_y;
	PN = (accumulator & 0x80) == 0x80;
	PZ = (accumulator == 0);
}