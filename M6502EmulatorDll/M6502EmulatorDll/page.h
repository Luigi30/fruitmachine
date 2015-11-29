#pragma once
#include "stdafx.h"
#include <stdint.h>

enum MEMORY_ACCESS { MEMORY_READ = 0x01, MEMORY_WRITE = 0x02 };

class Page {
public:
	char read_byte(uint8_t address);
	void write_byte(uint8_t address, uint8_t data);
	int getAccessType();

	Page() {
		for (int i = 0; i < 256; i++) {
			cells[i] = 0;
		}
	}

	Page(MEMORY_ACCESS access) {
		for (int i = 0; i < 256; i++) {
			cells[i] = 0;
		}
		access_type = access;
	}

private:
	uint8_t cells[256];
	int access_type = MEMORY_READ | MEMORY_WRITE;
};