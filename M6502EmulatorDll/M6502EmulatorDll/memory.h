#pragma once

#include <vector>
#include <iomanip>
#include <stdint.h>
#include <sstream>
#include <string>
#include <vector>

#include "WideAddress.h"
#include "page.h"

enum INDEX_MODE { INDEX_NONE = 0x01, INDEX_X = 0x02, INDEX_Y = 0x03 };

WideAddress uintAddressToWideAddress(uint16_t address);
uint16_t wideAddressToUintAddress(WideAddress address);

class MemoryMap {
private:
	Page pages[256];
	MemoryMap() {};
	MemoryMap(MemoryMap const&) {};
	MemoryMap& operator=(MemoryMap const&) {};
	static MemoryMap* m_pInstance;

public:
	static MemoryMap* Instance();
	uint8_t read_byte(uint16_t int_address);
	void write_byte(uint16_t int_address, uint8_t data);

	uint8_t read_byte(WideAddress address, bool overrides);
	void write_byte(WideAddress address, uint8_t data);
	void write_byte(WideAddress address, uint8_t data, bool overrides);

	uint8_t read_zero_page_byte(uint8_t int_address);
	void write_zero_page_byte(uint8_t int_address, uint8_t data);

	void initialize();

	void load_binary(uint16_t destination, std::string filename);

	void handleReadOverrides(WideAddress);
	void handleWriteOverrides(WideAddress);
};