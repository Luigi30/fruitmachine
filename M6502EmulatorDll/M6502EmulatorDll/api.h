#pragma once
#include "stdafx.h"
#include "mc6821.h"
#include <stdint.h>


#ifdef M6502EMULATORDLL_EXPORTS
#define M6502EMULATORDLL_API __declspec(dllexport)
#else
#define M6502EMULATORDLL_API __declspec(dllimport)
#endif

extern "C" {
	struct ProcessorStatus {
		uint16_t cycles; //cycle count
		uint8_t FLAG_SIGN;			//N
		uint8_t FLAG_OVERFLOW;		//V
								//-
		uint8_t FLAG_BREAKPOINT;	//B
		uint8_t FLAG_DECIMAL;		//D
		uint8_t FLAG_INTERRUPT;	//I
		uint8_t FLAG_ZERO;			//Z
		uint8_t FLAG_CARRY;		//C

		uint8_t accumulator;	//A
		uint8_t index_x;		//X
		uint8_t index_y;		//Y
		uint8_t stack_pointer;	//SP
		uint16_t program_counter; //PC
		uint16_t old_program_counter; //before this opcode ran

		LPCSTR last_opcode;
	};

	extern M6502EMULATORDLL_API bool loadBinary(const char *path, uint16_t address);
	
	//Processor information
	extern M6502EMULATORDLL_API UINT16 getProgramCounter();
	extern M6502EMULATORDLL_API ProcessorStatus getProcessorStatus();

	//Processor actions
	extern M6502EMULATORDLL_API void resetProcessor();
	extern M6502EMULATORDLL_API void doSingleStep();
	extern M6502EMULATORDLL_API void resetCycleCounter();

	//MC6821
	extern M6502EMULATORDLL_API MC6821Status getMC6821Status();
	extern M6502EMULATORDLL_API uint8_t getOutputBuffer();
	extern M6502EMULATORDLL_API void putKeyInBuffer(uint8_t);

	//Memory
	extern M6502EMULATORDLL_API uint8_t* getMemoryRange(uint16_t base, uint16_t length);
}