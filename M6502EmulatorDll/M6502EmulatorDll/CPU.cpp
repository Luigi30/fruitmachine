#include "stdafx.h"
#include "CPU.h"
#include <iostream>

/* 
	MOS 6502 CPU instruction set simulator.
	Passes the functional test ROMs I can find.
	
	TODO: Inaccurate cycle counts (cycle counter isn't incremented when zero-page access wraps, maybe some other things)
	TODO: Cycle-accurate timing instead of per-instruction timing (see above)
*/

//The master processor of instructions.
void CPU::process_instruction() {
	last_executed_opcode = "";
	last_operand = 0x00;
	old_program_counter = program_counter;
	uint8_t opcode = fetch_memory_byte(program_counter);
	execute_opcode(opcode);

	if (opcode == 0x4C) {
		loopCount++;
		if (loopCount == 10) {
			std::cout << "Infinite loop detected, aborting.";
			std::exit(3);
		}
	}
	else {
		loopCount = 0;
	}
}

void CPU::print_status(uint8_t opcode) {
	std::cout << "[EXE] Opcode " << std::hex << unsigned(opcode) << " (" << last_executed_opcode.c_str() << unsigned(last_operand) << "):" << std::endl;
	std::cout << "\tA=" << unsigned(accumulator) << std::setw(2) << " X=" << unsigned(index_x) << std::setw(2) << " Y=" << unsigned(index_y) << std::setw(2) << " SP=" << unsigned(stack_pointer) << " PC=" << unsigned(program_counter) << " ";
	
	if (FLAG_SIGN) {
		std::cout << "N";
	}
	else {
		std::cout << "-";
	}

	if (FLAG_OVERFLOW) {
		std::cout << "V";
	}
	else {
		std::cout << "-";
	}

	std::cout << "-";

	if (FLAG_BREAKPOINT) {
		std::cout << "B";
	}
	else {
		std::cout << "-";
	}

	if (FLAG_DECIMAL) {
		std::cout << "D";
	}
	else {
		std::cout << "-";
	}

	if (FLAG_INTERRUPT) {
		std::cout << "I";
	}
	else {
		std::cout << "-";
	}

	if (FLAG_ZERO) {
		std::cout << "Z";
	}
	else {
		std::cout << "-";
	}

	if (FLAG_CARRY) {
		std::cout << "C";
	}
	else {
		std::cout << "-";
	}

	std::cout << std::endl;

}

uint16_t CPU::getCycleCount() {
	return cycles;
}

void CPU::execute_opcode(uint8_t opcode)
{
	switch (opcode) {

		/* Addressing modes
		
			- ABC #		- Immediate - the operand is the next byte. advances PC by 2
			- ABC #$	- Accumulator. advances PC by 1
			- ABC ZP	- Zero-Page - the operand refers to a location in zero-page memory. advances PC by 2
			- ABC ZP,X	- Zero-Page, X/Y indexed. advances PC by 2. If ZP+X > $FF, wrap to $00 of zero-page memory.
			- ABC M		- Absolute - the operand is a 16-bit memory location. advances PC by 3
			- ABC M,X	- Absolute, X/Y indexed. advances PC by 2. if Abs+X or Abs+Y > $FF, do not wrap to the beginning of the page.
			- ABC (M)	- Indirect - the operand is a 16-bit pointer. advances PC by 3
			- ABC (M,X) - Indirect X-indexed. Calculate address from the pointer located at ZP+X. If ZP+X > $FF, wrap to $00 of zero-page memory.
			- ABC (M),Y - Indirect Y-indexed. The pointer is at ZP. Add Y to the dereferenced destination address. Do not wrap.
		*/

		case 0x00: //BRK
		{
			cycles += 7;
			last_executed_opcode = "BRK";
			program_counter++; //waste an instruction
			op_brk();
			break;
		}

		case 0x01:
		{
			cycles += 6;
			last_executed_opcode = "ORA (ZP,X) $";
			op_ora(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0x05:
		{
			cycles += 2;
			last_executed_opcode = "ORA $";
			op_ora(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x06:
		{
			cycles += 5;
			last_executed_opcode = "ASL $";
			op_asl(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x08: //PHP
		{
			cycles += 3;
			last_executed_opcode = "PHP";
			op_php();
			break;
		}

		case 0x09:
		{
			cycles += 2;
			last_executed_opcode = "ORA #$";
			op_ora(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x0A:
		{
			cycles += 2;
			last_executed_opcode = "ASL";
			op_asl();
			break;
		}

		case 0x0D:
		{
			cycles += 4;
			last_executed_opcode = "ORA $";

			uint8_t operand = fetch_operand(ADDRESSING_ABSOLUTE);
			accumulator = accumulator | operand;
			set_zero_and_sign_flags(accumulator);
			break;
		}

		case 0x0E:
		{
			cycles += 6;
			last_executed_opcode = "ASL $";
			op_asl(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x10:
		{
			cycles += 2; //2; +1 if dest on same page; +2 if on dest on diff page
			last_executed_opcode = "BPL $";
			op_bpl(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x11:
		{
			cycles += 5;
			last_executed_opcode = "ORA (M),Y $";
			op_ora(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0x15:
		{
			cycles += 4;
			last_executed_opcode = "ORA ZP,X";
			op_ora(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x16:
		{
			cycles += 6;
			last_executed_opcode = "ASL ZP,X $";
			op_asl(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x18: //CLC
		{
			cycles += 2;
			last_executed_opcode = "CLC";
			op_clc();
			break;
		}

		case 0x19:
		{
			cycles += 4;
			last_executed_opcode = "ORA M,Y $";
			op_ora(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0x1D:
		{
			cycles += 4;
			last_executed_opcode = "ORA M,X $";
			op_ora(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x1E:
		{
			cycles += 7;
			last_executed_opcode = "ASL M,X $";
			op_asl(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x20:
		{
			cycles += 6;
			last_executed_opcode = "JSR $";
			op_jsr(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x21: //AND (M, X)
		{
			cycles += 6;
			last_executed_opcode = "AND (M,X) $";
			op_and(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0x24:
		{
			cycles += 3;
			last_executed_opcode = "BIT $";
			op_bit(fetch_operand(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x25: //AND - AND zeropage
		{
			cycles += 2;
			last_executed_opcode = "AND $";
			op_and(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x26:
		{
			cycles += 5;
			last_executed_opcode = "ROL $";
			op_rol(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x28: //PLP
		{
			cycles += 4;
			last_executed_opcode = "PLP";
			op_plp();
			break;
		}

		case 0x29: //AND - AND(M, immediate)
		{
			cycles += 2;
			last_executed_opcode = "AND #$";
			op_and(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x2A:
		{
			cycles += 2;
			last_executed_opcode = "ROL A";
			op_rol();
			break;
		}

		case 0x2C:
		{
			cycles += 4;
			last_executed_opcode = "BIT $";
			op_bit(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x2D:
		{
			cycles += 4;
			last_executed_opcode = "AND $";
			op_and(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x2E:
		{
			cycles += 6;
			last_executed_opcode = "ROL $";
			op_rol(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x30:
		{
			cycles += 2;
			last_executed_opcode = "BMI $";
			op_bmi(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x31: //AND (M),Y
		{
			cycles += 5;
			last_executed_opcode = "AND (M),Y $";
			op_and(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0x35: //AND,X - AND(A, zeropage+X)
		{
			cycles += 3;
			last_executed_opcode = "AND ZP,X";
			op_and(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x36:
		{
			cycles += 6;
			last_executed_opcode = "ROL ZP,X $";
			op_rol(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x38: //SEC - set carry flag
		{
			cycles += 2;
			last_executed_opcode = "SEC";
			op_sec();
			break;
		}

		case 0x39: //AND M,Y
		{
			cycles += 4;
			last_executed_opcode = "AND M,Y $";
			op_and(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0x3D:
		{
			cycles += 4;
			last_executed_opcode = "AND M,X $";
			op_and(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}
		
		case 0x3E:
		{
			cycles += 7;
			last_executed_opcode = "ROL M,X $";
			op_rol(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x40:
		{
			cycles += 6;
			last_executed_opcode = "RTI";
			op_rti();
			break;
		}

		case 0x41:
		{
			cycles += 6;
			last_executed_opcode = "EOR (ZP,X) $";
			op_eor(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0x45:
		{
			cycles += 3;
			last_executed_opcode = "EOR $";
			op_eor(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x46:
		{
			cycles += 5;
			last_executed_opcode = "LSR $";
			op_lsr(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x48:
		{
			cycles += 3;
			last_executed_opcode = "PHA";
			op_pha();
			break;
		}

		case 0x49:
		{
			cycles += 2;
			last_executed_opcode = "EOR #$";
			op_eor(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x4A:
		{
			cycles += 2;
			last_executed_opcode = "LSR";
			op_lsr();
			break;
		}

		case 0x4C: //JMP - unconditional branch
		{
			cycles += 3;
			last_executed_opcode = "JMP $";
			op_jmp(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x4D:
		{
			cycles += 4;
			last_executed_opcode = "EOR $";
			op_eor(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x4E:
		{
			cycles += 6;
			last_executed_opcode = "LSR $";
			op_lsr(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x50:
		{
			cycles += 2;
			last_executed_opcode = "BVC";
			op_bvc(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x51:
		{
			cycles += 5;
			last_executed_opcode = "EOR (ZP),Y $";
			op_eor(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0x55:
		{
			cycles += 4;
			last_executed_opcode = "EOR ZP,X";
			op_eor(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x56:
		{
			cycles += 6;
			last_executed_opcode = "LSR ZP,X $";
			op_lsr(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x58: //CLI
		{
			cycles += 2;
			last_executed_opcode = "CLI";
			op_cli();
			break;
		}

		case 0x59:
		{
			cycles += 4;
			last_executed_opcode = "EOR M,Y $";
			op_eor(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0x5D:
		{
			cycles += 4;
			last_executed_opcode = "EOR M,X $";
			op_eor(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x5E:
		{
			cycles += 7;
			last_executed_opcode = "LSR M,X $";
			op_lsr(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x60:
		{
			cycles += 6;
			last_executed_opcode = "RTS";
			op_rts();
			break;
		}

		case 0x61:
		{
			cycles += 6;
			last_executed_opcode = "ADC (ZP,X) $";
			op_adc(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0x65:
		{
			cycles += 3;
			last_executed_opcode = "ADC ZP $";
			op_adc(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x66:
		{
			cycles += 5;
			last_executed_opcode = "ROR $";
			op_ror(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x68: //PLA - pop accumulator from stack
		{
			cycles += 4;
			last_executed_opcode = "PLA";
			op_pla();
			break;
		}

		case 0x69:
		{
			cycles += 2;
			last_executed_opcode = "ADC #$";
			op_adc(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x6A:
		{
			cycles += 2;
			last_executed_opcode = "ROR A";
			op_ror();
			break;
		}

		case 0x6C: //JMP (M)
		{
			cycles += 5;
			last_executed_opcode = "JMP (M) $";
			WideAddress pointer = fetch_operand_address(ADDRESSING_ABSOLUTE);
			WideAddress destination = dereference_indirect_address(pointer);
			op_jmp(destination);
			break;
		}

		case 0x6D:
		{
			cycles += 4;
			last_executed_opcode = "ADC M $";
			op_adc(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x6E:
		{
			cycles += 6;
			last_executed_opcode = "ROR $";
			op_ror(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x70:
		{
			cycles += 2;
			last_executed_opcode = "BVS $";
			op_bvs(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x71:
		{
			cycles += 5;
			last_executed_opcode = "ADC (ZP),Y $";
			op_adc(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0x75:
		{
			cycles += 4;
			last_executed_opcode = "ADC ZP,X $";
			op_adc(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x76:
		{
			cycles += 6;
			last_executed_opcode = "ROR ZP,X $";
			op_ror(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x78: //SEI - set interrupt flag
		{
			cycles += 2;
			last_executed_opcode = "SEI";
			op_sei();
			break;
		}

		case 0x7E:
		{
			cycles += 7;
			last_executed_opcode = "ROR $,X $";
			op_ror(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x79:
		{
			cycles += 4;
			last_executed_opcode = "ADC M,Y $";
			op_adc(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0x7D:
		{
			cycles += 4;
			last_executed_opcode = "ADC M,X $";
			op_adc(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0x81:
		{
			cycles += 6;
			last_executed_opcode = "STA (ZP,X)";
			op_sta(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0x84: //STY - store Y into zeropage memory - OK
		{
			cycles += 3;
			last_executed_opcode = "STY $";
			op_sty(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x85: //STA - store A into zeropage memory - OK
		{
			cycles += 3;
			last_executed_opcode = "STA $";
			op_sta(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x86: //STX - store X into zeropage memory - OK
		{
			cycles += 3;
			last_executed_opcode = "STX $";
			op_stx(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0x88: //DEY - OK
		{
			cycles += 2;
			last_executed_opcode = "DEY";
			op_dey();
			break;
		}

		case 0x8A: //TXA - X -> A - OK
		{
			cycles += 2;
			last_executed_opcode = "TXA";
			op_txa();
			break;
		}

		case 0x8C:
		{
			cycles += 4;
			last_executed_opcode = "STY $";
			op_sty(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x8D:
		{
			cycles += 4;
			last_executed_opcode = "STA $";
			op_sta(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x8E:
		{
			cycles += 4;
			last_executed_opcode = "STX $";
			op_stx(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0x90:
		{
			cycles += 2;
			last_executed_opcode = "BCC $";
			op_bcc(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0x91:
		{
			cycles += 6;
			last_executed_opcode = "STA (ZP),Y $";
			op_sta(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0x94:
		{
			cycles += 4;
			last_executed_opcode = "STY ZP,X $"; //OK
			op_sty(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x95:
		{
			cycles += 4;
			last_executed_opcode = "STA ZP,X $"; //OK
			op_sta(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0x96:
		{
			cycles += 4;
			last_executed_opcode = "STX ZP,Y $"; //OK
			op_stx(fetch_operand_address(ADDRESSING_ZEROPAGE_Y));
			break;
		}

		case 0x98: //TYA - Y -> A - OK
		{
			cycles += 2;
			last_executed_opcode = "TYA";
			op_tya();
			break;
		}

		case 0x99:
		{
			cycles += 5;
			last_executed_opcode = "STA M,Y $";
			op_sta(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0x9A: //TXS - X -> SP - OK
		{
			cycles += 2;
			last_executed_opcode = "TXS";
			op_txs();
			break;
		}

		case 0x9D:
		{
			cycles += 5;
			last_executed_opcode = "STA M,X $";
			op_sta(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0xA0: //LDY - immediate - OK
		{
			cycles += 2;
			last_executed_opcode = "LDY #$";
			op_ldy(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xA1:
		{
			cycles += 6;
			last_executed_opcode = "LDA (ZP,X)";
			op_lda(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0xA2: //LDX - immediate - OK
		{
			cycles += 2;
			last_executed_opcode = "LDX #$";
			op_ldx(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xA4:
		{
			cycles += 3;
			last_executed_opcode = "LDY $";
			op_ldy(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xA5: //LDA - Zeropage - Load A from zeropage operand.
		{
			cycles += 3;
			last_executed_opcode = "LDA $";
			op_lda(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xA6:
		{
			cycles += 3;
			last_executed_opcode = "LDX $";
			op_ldx(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xA8: //TAY - OK
		{
			cycles += 2;
			last_executed_opcode = "TAY";
			op_tay();
			break;
		}

		case 0xA9: //LDA - Immediate - Load A from immediate value - OK
		{
			cycles += 2;
			last_executed_opcode = "LDA #$";
			op_lda(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xAA: //TAX - A -> X - OK
		{
			cycles += 2;
			last_executed_opcode = "TAX";
			op_tax();
			break;
		}

		case 0xAC:
		{
			cycles += 4;
			last_executed_opcode = "LDY $";
			op_ldy(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xAD:
		{
			cycles += 4;
			last_executed_opcode = "LDA $";
			op_lda(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xAE:
		{
			cycles += 4;
			last_executed_opcode = "LDX $";
			op_ldx(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xB0:
		{
			cycles += 2;
			last_executed_opcode = "BCS $";
			op_bcs(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xB1:
		{
			cycles += 5;
			last_executed_opcode = "LDA (ZP),Y $";
			op_lda(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0xB4:
		{
			cycles += 4;
			last_executed_opcode = "LDY ZP,X $";
			op_ldy(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0xB5: //LDA,X - Load A from Zeropage+X value
		{
			cycles += 4;
			last_executed_opcode = "LDA ZP,X $";
			op_lda(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0xB6:
		{
			cycles += 4;
			last_executed_opcode = "LDX ZP,Y $";
			op_ldx(fetch_operand_address(ADDRESSING_ZEROPAGE_Y));
			break;
		}

		case 0xB8: //CLV - OK
		{
			cycles += 2;
			last_executed_opcode = "CLV";
			op_clv();
			break;
		}

		case 0xB9: //LDA Absolute,Y - OK
		{
			cycles += 4;
			last_executed_opcode = "LDA M,Y $";
			op_lda(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0xBA: //TSX - SP -> X - OK
		{
			cycles += 2;
			last_executed_opcode = "TSX";
			op_tsx();
			break;
		}

		case 0xBC:
		{
			cycles += 4;
			last_executed_opcode = "LDY M,X $";
			op_ldy(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0xBD: //LDA - Absolute,X - OK
		{
			cycles += 4;
			last_executed_opcode = "LDA M,X $";
			op_lda(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0xBE:
		{
			cycles += 4;
			last_executed_opcode = "LDX M,Y $";
			op_ldx(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0xC0:
		{
			cycles += 2;
			last_executed_opcode = "CPY #$";
			op_cpy(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xC1:
		{
			cycles += 6;
			last_executed_opcode = "CMP (ZP,X)";
			op_cmp(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0xC4:
		{
			cycles += 3;
			last_executed_opcode = "CPY $";
			op_cpy(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xC5:
		{
			cycles += 3;
			last_executed_opcode = "CMP $";
			op_cmp(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xC6:
		{
			cycles += 5;
			last_executed_opcode = "DEC $";
			op_dec(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xC8: //INY - increment Y - OK
		{
			cycles += 2;
			last_executed_opcode = "INY";
			op_iny();
			break;
		}

		case 0xC9:
		{
			cycles += 2;
			last_executed_opcode = "CMP #$";
			op_cmp(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xCA: //DEX - OK
		{
			cycles += 2;
			last_executed_opcode = "DEX";
			op_dex();
			break;
		}

		case 0xCC:
		{
			cycles += 4;
			last_executed_opcode = "CPY $";
			op_cpy(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xCD:
		{
			cycles += 4;
			last_executed_opcode = "CMP $";
			op_cmp(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xCE:
		{
			cycles += 6;
			last_executed_opcode = "DEC $";
			op_dec(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xD0: //BNE
		{
			cycles += 2;
			last_executed_opcode = "BNE $";
			op_bne(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xD1:
		{
			cycles += 5;
			last_executed_opcode = "CMP (ZP),Y";
			op_cmp(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0xD5:
		{
			cycles += 4;
			last_executed_opcode = "CMP ZP,X $";
			op_cmp(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0xD6:
		{
			cycles += 6;
			last_executed_opcode = "DEC ZP,X $";
			op_dec(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0xD8: //CLD - OK
		{
			cycles += 2;
			last_executed_opcode = "CLD";
			op_cld();
			break;
		}

		case 0xD9:
		{
			cycles += 4;
			last_executed_opcode = "CMP M,Y $";
			op_cmp(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0xDD:
		{
			cycles += 4;
			last_executed_opcode = "CMP M,X $";
			op_cmp(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0xDE:
		{
			cycles += 7;
			last_executed_opcode = "DEC M,X $";
			op_dec(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0xE0:
		{
			cycles += 2;
			last_executed_opcode = "CPX #$";
			op_cpx(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xE1:
		{
			cycles += 6;
			last_executed_opcode = "SBC (ZP,X) $";
			op_sbc(fetch_operand_address(ADDRESSING_INDIRECT_X));
			break;
		}

		case 0xE4:
		{
			cycles += 3;
			last_executed_opcode = "CPX $";
			op_cpx(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xE5:
		{
			cycles += 3;
			last_executed_opcode = "SBC $";
			op_sbc(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xE6: //INC, increment zero-page memory address
		{
			cycles += 5;
			last_executed_opcode = "INC $";
			op_inc(fetch_operand_address(ADDRESSING_ZEROPAGE));
			break;
		}

		case 0xE8: //INX - increment X - OK
		{
			cycles += 2;
			last_executed_opcode = "INX";
			op_inx();
			break;
		}

		case 0xE9:
		{
			cycles += 2;
			last_executed_opcode = "SBC #$";
			op_sbc(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xEA: //NOP - OK
		{
			cycles += 2;
			last_executed_opcode = "NOP";
			break;
		}

		case 0xEC:
		{
			cycles += 4;
			last_executed_opcode = "CPX $";
			op_cpx(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xED:
		{
			cycles += 4;
			last_executed_opcode = "SBC $";
			op_sbc(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xEE:
		{
			cycles += 6;
			last_executed_opcode = "INC $";
			op_inc(fetch_operand_address(ADDRESSING_ABSOLUTE));
			break;
		}

		case 0xF0:
		{
			cycles += 2;
			last_executed_opcode = "BEQ $";
			op_beq(fetch_operand(ADDRESSING_IMMEDIATE));
			break;
		}

		case 0xF1:
		{
			cycles += 5;
			last_executed_opcode = "SBC (ZP),Y $";
			op_sbc(fetch_operand_address(ADDRESSING_INDIRECT_Y));
			break;
		}

		case 0xF5:
		{
			cycles += 4;
			last_executed_opcode = "SBC ZP,X $";
			op_sbc(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0xF6:
		{
			cycles += 6;
			last_executed_opcode = "INC ZP,X $";
			op_inc(fetch_operand_address(ADDRESSING_ZEROPAGE_X));
			break;
		}

		case 0xF8: //SED - set decimal flag - OK
		{	
			cycles += 2;
			last_executed_opcode = "SED";
			op_sed();
			break;
		}

		case 0xF9:
		{
			cycles += 4;
			last_executed_opcode = "SBC M,Y $";
			op_sbc(fetch_operand_address(ADDRESSING_ABSOLUTE_Y));
			break;
		}

		case 0xFD:
		{
			cycles += 4;
			last_executed_opcode = "SBC M,X $";
			op_sbc(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}

		case 0xFE:
		{
			cycles += 7;
			last_executed_opcode = "INC M,X $";
			op_inc(fetch_operand_address(ADDRESSING_ABSOLUTE_X));
			break;
		}
		
		default:
			printf("Illegal instruction %x! Aborting execution.", opcode);
			std::exit(2);
	}
}

//check some flags
void CPU::set_zero_and_sign_flags(uint8_t data) {
	if (data == 0) {
		FLAG_ZERO = true;
	}
	else {
		FLAG_ZERO = false;
	}
	if (data & 0x80) {
		FLAG_SIGN = true;
	}
	else {
		FLAG_SIGN = false;
	}
}

/* Stack */

//Push a value onto the stack and decrement SP.
void CPU::stack_push(uint8_t data) {
	MemoryMap::Instance()->write_byte(0x0100 + stack_pointer, data);
	stack_pointer--;
}

//Increment SP and pop the value we find.
uint8_t CPU::stack_pop() {
	stack_pointer++;
	uint8_t data = MemoryMap::Instance()->read_byte(0x0100 + stack_pointer);
	return data;
}

/* Memory read/write */

uint8_t CPU::fetch_memory_byte(WideAddress address, bool incrementPc) {

	if (incrementPc) {
		program_counter++;
	}

	return MemoryMap::Instance()->read_byte(address);
}

uint8_t CPU::fetch_memory_byte(uint16_t address, bool incrementPc) {
	return fetch_memory_byte(uintAddressToWideAddress(address), incrementPc);
}

uint8_t CPU::fetch_zero_page_byte(uint8_t address) {
	return MemoryMap::Instance()->read_zero_page_byte(address);
}

void CPU::write_memory_byte(uint16_t address, uint8_t data) {
	MemoryMap::Instance()->write_byte(address, data);
}

void CPU::write_memory_byte(WideAddress address, uint8_t data) {
	MemoryMap::Instance()->write_byte(address, data);
}

void CPU::write_zero_page_byte(uint8_t address, uint8_t data) {
	MemoryMap::Instance()->write_zero_page_byte(address, data);
}

/* PSW */

uint8_t CPU::getProgramStatusWord() {
	uint8_t psw = 0x20;

	if (FLAG_SIGN) {
		psw += 0x80;
	}

	if (FLAG_OVERFLOW) {
		psw += 0x40;
	}

	if (FLAG_BREAKPOINT) {
		psw += 0x10;
	}

	if (FLAG_DECIMAL) {
		psw += 0x08;
	}

	if (FLAG_INTERRUPT) {
		psw += 0x04;
	}

	if (FLAG_ZERO) {
		psw += 0x02;
	}

	if (FLAG_CARRY) {
		psw += 0x01;
	}

	return psw;
}

void CPU::setProgramStatusWord(uint8_t psw) {
	FLAG_SIGN = ((0x80 & psw) == 0x80);
	FLAG_OVERFLOW = ((0x40 & psw) == 0x40);
	FLAG_BREAKPOINT = ((0x10 & psw) == 0x10);
	FLAG_DECIMAL = ((0x08 & psw) == 0x08);
	FLAG_INTERRUPT = ((0x04 & psw) == 0x04);
	FLAG_ZERO = ((0x02 & psw) == 0x02);
	FLAG_CARRY = ((0x01 & psw) == 0x01);
}

/* Debug */

uint8_t CPU::getAccumulator() {
	return accumulator;
}

bool CPU::frameInstructionsComplete() {
	return cycles >= maxCycles;
}

void CPU::resetCycleCounter() {
	cycles = 0;
}

/* Interop */
bool CPU::getSignFlag() {
	return FLAG_SIGN;
}
bool CPU::getOverflowFlag() {
	return FLAG_OVERFLOW;
}
bool CPU::getBreakpointFlag() {
	return FLAG_BREAKPOINT;
}
bool CPU::getDecimalFlag() {
	return FLAG_DECIMAL;
}
bool CPU::getInterruptFlag() {
	return FLAG_INTERRUPT;
}
bool CPU::getZeroFlag() {
	return FLAG_ZERO;
}
bool CPU::getCarryFlag() {
	return FLAG_CARRY;
}

uint8_t CPU::getIndexX() {
	return index_x;
}
uint8_t CPU::getIndexY() {
	return index_y;
}
uint8_t CPU::getStackPointer() {
	return stack_pointer;
}
uint16_t CPU::getProgramCounter() {
	return program_counter;
}
uint16_t CPU::getOldProgramCounter() {
	return old_program_counter;
}