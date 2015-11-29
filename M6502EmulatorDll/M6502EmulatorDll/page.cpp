#include "stdafx.h"
#include <stdint.h>
#include "page.h"

char Page::read_byte(uint8_t address) {
	return cells[address];
}

void Page::write_byte(uint8_t address, uint8_t data) {
	cells[address] = data;
}

int Page::getAccessType() {
	return access_type;
}