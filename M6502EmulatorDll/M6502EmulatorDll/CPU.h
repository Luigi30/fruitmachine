#include <stdint.h>
#include <vector>
#include "memory.h"

#pragma once
class CPU {

private:

	//PSW
	bool FLAG_SIGN; //N
	bool FLAG_OVERFLOW; //V
	//-
	bool FLAG_BREAKPOINT; //B
	bool FLAG_DECIMAL; //D
	bool FLAG_INTERRUPT; //I
	bool FLAG_ZERO; //Z
	bool FLAG_CARRY; //C

	//registers
	uint8_t accumulator; //A
	uint8_t index_x; //X
	uint8_t index_y; //Y
	uint8_t stack_pointer; //SP
	uint16_t program_counter; //PC
	uint16_t old_program_counter; //before this opcode ran

	uint16_t last_operand;

	int loopCount = 0;
	int cycles = 0;
	int maxCycles = 17050; //roughly 1MHz

public:
	std::string last_executed_opcode;
	std::vector<std::string> last_executed_opcodes;

	enum ADDRESSING_MODE { ADDRESSING_IMMEDIATE, ADDRESSING_ABSOLUTE, ADDRESSING_ABSOLUTE_X, ADDRESSING_ABSOLUTE_Y, ADDRESSING_ZEROPAGE, ADDRESSING_ZEROPAGE_X, ADDRESSING_ZEROPAGE_Y, ADDRESSING_INDIRECT, ADDRESSING_INDIRECT_X, ADDRESSING_INDIRECT_Y };
	enum INDEX_MODE { INDEX_NONE = 0x01, INDEX_X = 0x02, INDEX_Y = 0x03 };

	static bool lastInstructionCrossedPageBoundary;

	bool frameInstructionsComplete();
	void resetCycleCounter();

	void reset_processor() {
		lastInstructionCrossedPageBoundary = false;
		FLAG_SIGN = false;
		FLAG_OVERFLOW = false;
		FLAG_BREAKPOINT = false;
		FLAG_DECIMAL = false;
		FLAG_INTERRUPT = true;
		FLAG_ZERO = false;
		FLAG_CARRY = false;
		accumulator = 0;
		index_x = 0;
		index_y = 0;
		stack_pointer = 0xFF;
		program_counter = 0xFFFC; //apple
		//program_counter = 0x0400; //test

		execute_opcode(0x4c); //jump to reset vector destination
	}

	uint8_t getAccumulator();
	void setAccumulator(uint8_t acc) {
		accumulator = acc;
	}

	uint8_t getCarry() {
		return FLAG_CARRY;
	}
	void setCarry(bool val) {
		FLAG_CARRY = val;
	}

	bool getSignFlag();
	bool getOverflowFlag();
	bool getBreakpointFlag();
	bool getDecimalFlag();
	bool getInterruptFlag();
	bool getZeroFlag();
	bool getCarryFlag();
	
	uint8_t getIndexX();
	uint8_t getIndexY();
	uint8_t getStackPointer();
	uint16_t getProgramCounter();
	uint16_t getOldProgramCounter();
	uint16_t getCycleCount();

	uint8_t getProgramStatusWord();
	void setProgramStatusWord(uint8_t psw);

	void process_instruction();
	void execute_opcode(uint8_t opcode);
	void set_zero_and_sign_flags(uint8_t data);

	void stack_push(uint8_t data);
	uint8_t stack_pop();

	uint8_t fetch_memory_byte(WideAddress address, bool incrementPc = true);
	uint8_t fetch_memory_byte(uint16_t address, bool incrementPc = true);
	uint8_t fetch_zero_page_byte(uint8_t);


	void write_memory_byte(uint16_t, uint8_t);
	void write_memory_byte(WideAddress, uint8_t);
	void write_zero_page_byte(uint8_t, uint8_t);

	WideAddress fetch_dereferenced_zero_page_pointer(INDEX_MODE mode);
	WideAddress dereference_indirect_address(uint8_t zero_page_pointer);
	WideAddress dereference_indirect_address(WideAddress pointer);

	uint8_t fetch_operand(ADDRESSING_MODE);
	WideAddress fetch_operand_address(ADDRESSING_MODE);

	uint8_t fetch_destination(ADDRESSING_MODE mode);

	void print_status(uint8_t opcode);

	/* Opcodes */
	void CPU::op_adc(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_adc(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_and(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_and(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_asl(WideAddress address);

	void CPU::op_asl();

	void CPU::op_bcc(int8_t relative);

	void CPU::op_bcs(int8_t relative);

	void CPU::op_beq(int8_t relative);

	void CPU::op_bit(WideAddress address);

	void CPU::op_bmi(int8_t relative);

	void CPU::op_bne(int8_t relative);

	void CPU::op_bpl(int8_t relative);

	void CPU::op_brk();

	void CPU::op_bvc(int8_t relative);

	void CPU::op_bvs(int8_t relative);

	void CPU::op_clc();

	void CPU::op_cld();

	void CPU::op_cli();

	void CPU::op_clv();

	void CPU::op_cmp(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_cmp(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_cpx(uint8_t immediate);

	void CPU::op_cpx(WideAddress address);

	void CPU::op_cpy(uint8_t immediate);

	void CPU::op_cpy(WideAddress address);

	void CPU::op_dec(WideAddress address);

	void CPU::op_dex();

	void CPU::op_dey();

	void CPU::op_eor(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_eor(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_inc(WideAddress address);

	void CPU::op_inx();

	void CPU::op_iny();

	void CPU::op_jmp(WideAddress address);

	void CPU::op_jsr(WideAddress address);

	void CPU::op_lda(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_lda(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_ldx(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_ldx(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_ldy(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_ldy(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_lsr();

	void CPU::op_lsr(WideAddress address);

	void CPU::op_nop();

	void CPU::op_ora(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_ora(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_pha();

	void CPU::op_php();

	void CPU::op_pla();

	void CPU::op_plp();

	void CPU::op_rol();

	void CPU::op_rol(WideAddress address);

	void CPU::op_ror();

	void CPU::op_ror(WideAddress address);

	void CPU::op_rti();

	void CPU::op_rts();

	void CPU::op_sbc(uint8_t immediate, bool crossedPageBoundary = false);

	void CPU::op_sbc(WideAddress address, bool crossedPageBoundary = false);

	void CPU::op_sec();

	void CPU::op_sed();

	void CPU::op_sei();

	void CPU::op_sta(WideAddress address);

	void CPU::op_stx(WideAddress address);

	void CPU::op_sty(WideAddress address);

	void CPU::op_tax();

	void CPU::op_tay();

	void CPU::op_tsx();

	void CPU::op_txa();

	void CPU::op_txs();

	void CPU::op_tya();
};