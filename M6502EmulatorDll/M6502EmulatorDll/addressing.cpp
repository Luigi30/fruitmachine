#include "stdafx.h"
#include "CPU.h"
#include "memory.h"
#include <iostream>

uint8_t CPU::fetch_operand(ADDRESSING_MODE mode) {
	lastInstructionCrossedPageBoundary = false;

	switch (mode) {
	case ADDRESSING_IMMEDIATE:
	{
		uint8_t operand = fetch_memory_byte(program_counter, true);
		last_operand = operand;
		return operand;
	}

	case ADDRESSING_ZEROPAGE:
	{
		//TODO: Why is this here?
		uint8_t address = fetch_operand(ADDRESSING_IMMEDIATE);
		last_operand = address;
		return address;
	}

	case ADDRESSING_ZEROPAGE_X:
	{
		uint8_t address = fetch_operand(ADDRESSING_IMMEDIATE);
		lastInstructionCrossedPageBoundary = (address + index_x < address);
		uint8_t operand = fetch_zero_page_byte(address + index_x);
		last_operand = address;
		return operand;
	}

	case ADDRESSING_ZEROPAGE_Y:
	{
		uint8_t address = fetch_operand(ADDRESSING_IMMEDIATE);
		lastInstructionCrossedPageBoundary = (address + index_y < address);
		uint8_t operand = fetch_zero_page_byte(address + index_y);
		last_operand = address;
		return operand;
	}

	case ADDRESSING_ABSOLUTE:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		uint8_t addrHigh = fetch_memory_byte(program_counter);
		WideAddress address = { addrHigh, addrLow };
		uint8_t operand = fetch_memory_byte(address, false);
		last_operand = address;
		return operand;
	}

	case ADDRESSING_ABSOLUTE_X:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		uint8_t addrHigh = fetch_memory_byte(program_counter);
		WideAddress address = { addrHigh, addrLow };
		lastInstructionCrossedPageBoundary = (addrLow + index_x < addrLow);
		uint8_t operand = fetch_memory_byte(address.add(index_x, false), false);
		last_operand = address;
		return operand;
	}

	case ADDRESSING_ABSOLUTE_Y:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		uint8_t addrHigh = fetch_memory_byte(program_counter);
		WideAddress address = { addrHigh, addrLow };
		lastInstructionCrossedPageBoundary = (addrLow + index_y < addrLow);
		uint8_t operand = fetch_memory_byte(address.add(index_y, false), false);
		last_operand = address;
		return operand;
	}

	case ADDRESSING_INDIRECT_X:
	{
		WideAddress operand_address = fetch_dereferenced_zero_page_pointer(INDEX_X);
		last_operand = operand_address;
		uint8_t operand = fetch_memory_byte(operand_address, false);
		last_operand = operand;
		return operand;
	}

	case ADDRESSING_INDIRECT_Y:
	{
		WideAddress destination = fetch_dereferenced_zero_page_pointer(INDEX_NONE);
		lastInstructionCrossedPageBoundary = ((destination & 0xFF00) + index_y) > (destination & 0xFF00);
		destination = destination.add(index_y, false);
		uint8_t operand = fetch_memory_byte(destination);
		last_operand = operand;
		return operand;
	}
	default:
	{
		printf("fetch_operand(): Invalid addressing mode %d", mode);
		std::exit(255);
	}
	}

}

WideAddress CPU::fetch_operand_address(ADDRESSING_MODE mode) {
	switch (mode) {
	case ADDRESSING_ABSOLUTE:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		uint8_t addrHigh = fetch_memory_byte(program_counter);
		WideAddress address = { addrHigh, addrLow };
		last_operand = address;
		return address;
	}
	case ADDRESSING_ABSOLUTE_X:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		uint8_t addrHigh = fetch_memory_byte(program_counter);
		WideAddress address = { addrHigh, addrLow };
		last_operand = address;
		address = address.add(index_x, false);
		return address;
	}
	case ADDRESSING_ABSOLUTE_Y:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		uint8_t addrHigh = fetch_memory_byte(program_counter);
		WideAddress address = { addrHigh, addrLow };
		last_operand = address;
		address = address.add(index_y, false);
		return address;
	}
	case ADDRESSING_ZEROPAGE:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		WideAddress address = { 0, addrLow };
		last_operand = address;
		return address;
	}
	case ADDRESSING_ZEROPAGE_X:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		WideAddress address = { 0, uint8_t(addrLow + index_x) };
		last_operand = addrLow;
		return address;
	}
	case ADDRESSING_ZEROPAGE_Y:
	{
		uint8_t addrLow = fetch_memory_byte(program_counter);
		WideAddress address = { 0, uint8_t(addrLow + index_y) };
		last_operand = addrLow;
		return address;
	}
	case ADDRESSING_INDIRECT:
	{
		uint8_t zpAddr = fetch_memory_byte(program_counter);
		last_operand = zpAddr;
		return dereference_indirect_address(zpAddr);

	}
	case ADDRESSING_INDIRECT_X: //(ZP,X)
	{
		uint8_t zpAddr = fetch_memory_byte(program_counter);
		last_operand = zpAddr;
		return dereference_indirect_address(zpAddr + index_x);
	}
	case ADDRESSING_INDIRECT_Y: //(ZP),Y
	{
		uint8_t zpAddr = fetch_memory_byte(program_counter);
		last_operand = zpAddr;
		return dereference_indirect_address(zpAddr).add(index_y, false);
	}
	default:
	{
		printf("fetch_operand_address(): Invalid addressing mode %d", mode);
		std::exit(255);
	}
	}
}

uint8_t CPU::fetch_destination(ADDRESSING_MODE mode) {
	switch (mode) {

	case ADDRESSING_ZEROPAGE:
	{
		return fetch_operand(ADDRESSING_IMMEDIATE);
	}
	case ADDRESSING_ZEROPAGE_X:
	{
		return fetch_operand(ADDRESSING_IMMEDIATE) + index_x;
	}
	case ADDRESSING_ZEROPAGE_Y:
	{
		return fetch_operand(ADDRESSING_IMMEDIATE) + index_y;
	}
	default:
	{
		printf("fetch_destination(): Invalid addressing mode %d", mode);
		std::exit(255);
	}

	}
}

WideAddress CPU::dereference_indirect_address(uint8_t zero_page_pointer) {
	uint8_t addressLow = fetch_zero_page_byte(zero_page_pointer);
	uint8_t addressHigh = fetch_zero_page_byte(zero_page_pointer + 1); //will wrap
	return WideAddress{ addressHigh, addressLow };
}

WideAddress CPU::dereference_indirect_address(WideAddress pointer) {
	uint8_t lo = fetch_memory_byte(pointer, false);
	uint8_t hi = fetch_memory_byte(pointer.add(1, false), false);
	WideAddress dereferenced = WideAddress{ hi, lo };
	return dereferenced;
}

WideAddress CPU::fetch_dereferenced_zero_page_pointer(INDEX_MODE mode) { //(ZP) and (ZP,X)
	uint8_t addrLow;
	uint8_t addrHigh;
	uint8_t zero_page_address = fetch_memory_byte(program_counter);

	if (mode == INDEX_X) {
		addrLow = fetch_zero_page_byte(zero_page_address + index_x);
		addrHigh = fetch_zero_page_byte(zero_page_address + index_x + 1);
	}
	else if (mode == INDEX_NONE) {
		addrLow = fetch_zero_page_byte(zero_page_address);
		addrHigh = fetch_zero_page_byte(zero_page_address + 1);
	}
	else {
		std::cout << "fetch_dereferenced_zero_page_pointer(): Implementation error, cannot use INDEX_Y mode with this function.";
		std::exit(255);
	}

	WideAddress address = WideAddress{ addrHigh, addrLow };
	return address;

}