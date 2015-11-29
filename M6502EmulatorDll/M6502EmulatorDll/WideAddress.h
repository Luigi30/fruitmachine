#pragma once
#include "stdafx.h"
#include <stdint.h>

struct WideAddress {
	uint8_t high;
	uint8_t low;

	WideAddress add(uint8_t operand, bool wrap) {
		if (wrap) {
			low += operand;
		}
		else
		{
			uint8_t intermediate = low + operand;

			if (intermediate < low) {
				high++;
				low += operand;
			}
			else {
				low += operand;
			}
		}

		return *this;
	}

	WideAddress(uint8_t hi, uint8_t lo) {
		high = hi;
		low = lo;
	}

	WideAddress(uint8_t lo) {
		high = 0;
		low = lo;
	}

	WideAddress() {
		high = 0;
		low = 0;
	}

	operator uint16_t() {
		return this->low | (this->high << 8);
	}

};